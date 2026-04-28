#include <QtTest>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>

#include "core/ringbuffer.h"
#include "core/samplepack.h"
#include "protocol/protocolframe.h"
#include "protocol/protocolparser.h"
#include "storage/csvrecorder.h"

class CoreTest : public QObject
{
    Q_OBJECT

private slots:
    void samplePack_reportsValuesAndChannelCount();
    void samplePack_returnsZeroForOutOfRangeChannel();
    void samplePack_defaultConstructsAsEmpty();

    void ringBuffer_keepsValuesWithinCapacity();
    void ringBuffer_resizesDownKeepingNewestValues();
    void ringBuffer_clampsInvalidCapacityToOne();
    void ringBuffer_clearRemovesAllValues();

    void protocolFrame_defaultStateIsInvalid();
    void protocolParser_parsesValidFrame();
    void protocolParser_handlesSplitFramesAcrossChunks();
    void protocolParser_parsesMultipleFramesFromOneChunk();
    void protocolParser_rejectsInvalidChecksum();
    void protocolParser_countsLostPacketsFromSequenceGaps();
    void protocolParser_ignoresOutOfOrderFramesForLossBaseline();
    void protocolParser_resetsSequenceBaselineWhenDeviceTimestampRestarts();
    void csvRecorder_createsFileOnStart();
    void csvRecorder_writesHeaderAndSingleFrame();
    void csvRecorder_appendsMultipleFrames();
    void csvRecorder_ignoresAppendsAfterStop();
    void csvRecorder_escapesRawFrameForCsv();
};

void CoreTest::samplePack_reportsValuesAndChannelCount()
{
    const SamplePack pack(QVector<double>{1.25, -2.5, 3.75});

    QCOMPARE(pack.channelCount(), 3);
    QVERIFY(!pack.isEmpty());
    QCOMPARE(pack.value(0), 1.25);
    QCOMPARE(pack.value(1), -2.5);
    QCOMPARE(pack.value(2), 3.75);
    QCOMPARE(pack.values(), QVector<double>({1.25, -2.5, 3.75}));
}

void CoreTest::samplePack_returnsZeroForOutOfRangeChannel()
{
    const SamplePack pack(QVector<double>{4.0});

    QCOMPARE(pack.value(-1), 0.0);
    QCOMPARE(pack.value(1), 0.0);
}

void CoreTest::samplePack_defaultConstructsAsEmpty()
{
    const SamplePack pack;

    QCOMPARE(pack.channelCount(), 0);
    QVERIFY(pack.isEmpty());
    QVERIFY(pack.values().isEmpty());
}

void CoreTest::ringBuffer_keepsValuesWithinCapacity()
{
    RingBuffer buffer(3);

    buffer.append(1.0);
    buffer.append(2.0);
    buffer.append(3.0);
    buffer.append(4.0);

    QCOMPARE(buffer.capacity(), 3);
    QCOMPARE(buffer.size(), 3);
    QCOMPARE(buffer.values(), QVector<double>({2.0, 3.0, 4.0}));
}

void CoreTest::ringBuffer_resizesDownKeepingNewestValues()
{
    RingBuffer buffer(5);
    buffer.append(1.0);
    buffer.append(2.0);
    buffer.append(3.0);
    buffer.append(4.0);

    buffer.setCapacity(2);

    QCOMPARE(buffer.capacity(), 2);
    QCOMPARE(buffer.size(), 2);
    QCOMPARE(buffer.values(), QVector<double>({3.0, 4.0}));
}

void CoreTest::ringBuffer_clampsInvalidCapacityToOne()
{
    RingBuffer buffer(0);

    buffer.append(1.0);
    buffer.append(2.0);

    QCOMPARE(buffer.capacity(), 1);
    QCOMPARE(buffer.size(), 1);
    QCOMPARE(buffer.values(), QVector<double>({2.0}));

    buffer.setCapacity(-10);
    buffer.append(3.0);

    QCOMPARE(buffer.capacity(), 1);
    QCOMPARE(buffer.size(), 1);
    QCOMPARE(buffer.values(), QVector<double>({3.0}));
}

void CoreTest::ringBuffer_clearRemovesAllValues()
{
    RingBuffer buffer(3);
    buffer.append(1.0);
    buffer.append(2.0);

    buffer.clear();

    QVERIFY(buffer.isEmpty());
    QCOMPARE(buffer.size(), 0);
    QVERIFY(buffer.values().isEmpty());
}

void CoreTest::protocolFrame_defaultStateIsInvalid()
{
    const ProtocolFrame frame;

    QVERIFY(!frame.isValid);
    QCOMPARE(frame.sequence, static_cast<quint32>(0));
    QCOMPARE(frame.timestampMs, static_cast<quint64>(0));
    QVERIFY(frame.values.isEmpty());
    QVERIFY(frame.rawFrame.isEmpty());
}

void CoreTest::protocolParser_parsesValidFrame()
{
    ProtocolParser parser;

    parser.appendData("$DATA,15,123456,25.60,60.20,3.31*25\r\n");
    const QVector<ProtocolFrame> frames = parser.takeFrames();

    QCOMPARE(parser.parseErrorCount(), 0);
    QCOMPARE(parser.checksumErrorCount(), 0);
    QCOMPARE(frames.size(), 1);
    QVERIFY(frames[0].isValid);
    QCOMPARE(frames[0].sequence, static_cast<quint32>(15));
    QCOMPARE(frames[0].timestampMs, static_cast<quint64>(123456));
    QCOMPARE(frames[0].values, QVector<double>({25.60, 60.20, 3.31}));
    QCOMPARE(frames[0].rawFrame, QByteArray("$DATA,15,123456,25.60,60.20,3.31*25"));
    QCOMPARE(parser.parseErrorCount(), 0);
    QCOMPARE(parser.checksumErrorCount(), 0);
    QCOMPARE(parser.lostPacketCount(), 0);
}

void CoreTest::protocolParser_handlesSplitFramesAcrossChunks()
{
    ProtocolParser parser;

    parser.appendData("$DATA,1,10,1.0");
    QVERIFY(parser.takeFrames().isEmpty());

    parser.appendData(",2.0,3.0*22\r\n");
    const QVector<ProtocolFrame> frames = parser.takeFrames();

    QCOMPARE(parser.parseErrorCount(), 0);
    QCOMPARE(parser.checksumErrorCount(), 0);
    QCOMPARE(frames.size(), 1);
    QVERIFY(frames[0].isValid);
    QCOMPARE(frames[0].sequence, static_cast<quint32>(1));
    QCOMPARE(frames[0].timestampMs, static_cast<quint64>(10));
    QCOMPARE(frames[0].values, QVector<double>({1.0, 2.0, 3.0}));
}

void CoreTest::protocolParser_parsesMultipleFramesFromOneChunk()
{
    ProtocolParser parser;

    parser.appendData(
        "$DATA,1,10,1.0,2.0,3.0*22\r\n"
        "$DATA,2,20,4.0,5.0,6.0*25\r\n"
    );
    const QVector<ProtocolFrame> frames = parser.takeFrames();

    QCOMPARE(parser.parseErrorCount(), 0);
    QCOMPARE(parser.checksumErrorCount(), 0);
    QCOMPARE(frames.size(), 2);
    QCOMPARE(frames[0].sequence, static_cast<quint32>(1));
    QCOMPARE(frames[1].sequence, static_cast<quint32>(2));
}

void CoreTest::protocolParser_rejectsInvalidChecksum()
{
    ProtocolParser parser;

    parser.appendData("$DATA,1,10,1.0,2.0,3.0*00\r\n");

    QVERIFY(parser.takeFrames().isEmpty());
    QCOMPARE(parser.parseErrorCount(), 0);
    QCOMPARE(parser.checksumErrorCount(), 1);
}

void CoreTest::protocolParser_countsLostPacketsFromSequenceGaps()
{
    ProtocolParser parser;

    parser.appendData(
        "$DATA,10,10,1.0,2.0,3.0*12\r\n"
        "$DATA,13,20,4.0,5.0,6.0*15\r\n"
    );
    const QVector<ProtocolFrame> frames = parser.takeFrames();

    QCOMPARE(parser.parseErrorCount(), 0);
    QCOMPARE(parser.checksumErrorCount(), 0);
    QCOMPARE(frames.size(), 2);
    QCOMPARE(parser.lostPacketCount(), 2);
}

void CoreTest::protocolParser_ignoresOutOfOrderFramesForLossBaseline()
{
    ProtocolParser parser;

    parser.appendData(
        "$DATA,10,100,1.0,2.0,3.0*22\r\n"
        "$DATA,8,120,4.0,5.0,6.0*1E\r\n"
        "$DATA,11,140,7.0,8.0,9.0*21\r\n"
    );
    const QVector<ProtocolFrame> frames = parser.takeFrames();

    QCOMPARE(parser.parseErrorCount(), 0);
    QCOMPARE(parser.checksumErrorCount(), 0);
    QCOMPARE(frames.size(), 3);
    QCOMPARE(frames[0].sequence, static_cast<quint32>(10));
    QCOMPARE(frames[1].sequence, static_cast<quint32>(8));
    QCOMPARE(frames[2].sequence, static_cast<quint32>(11));
    QCOMPARE(parser.lostPacketCount(), 0);
}

void CoreTest::protocolParser_resetsSequenceBaselineWhenDeviceTimestampRestarts()
{
    ProtocolParser parser;

    parser.appendData(
        "$DATA,100,5000,1.0,2.0,3.0*26\r\n"
        "$DATA,0,20,4.0,5.0,6.0*27\r\n"
        "$DATA,1,40,7.0,8.0,9.0*21\r\n"
    );
    const QVector<ProtocolFrame> frames = parser.takeFrames();

    QCOMPARE(parser.parseErrorCount(), 0);
    QCOMPARE(parser.checksumErrorCount(), 0);
    QCOMPARE(frames.size(), 3);
    QCOMPARE(frames[0].sequence, static_cast<quint32>(100));
    QCOMPARE(frames[1].sequence, static_cast<quint32>(0));
    QCOMPARE(frames[2].sequence, static_cast<quint32>(1));
    QCOMPARE(parser.lostPacketCount(), 0);
}

void CoreTest::csvRecorder_createsFileOnStart()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    CsvRecorder recorder;
    const QString filePath = tempDir.filePath("session_test.csv");

    QVERIFY(recorder.start(filePath));
    QVERIFY(recorder.isRecording());
    QCOMPARE(recorder.filePath(), filePath);
    QVERIFY(QFile::exists(filePath));

    recorder.stop();
    QVERIFY(!recorder.isRecording());
}

void CoreTest::csvRecorder_writesHeaderAndSingleFrame()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    CsvRecorder recorder;
    const QString filePath = tempDir.filePath("single_frame.csv");
    QVERIFY(recorder.start(filePath));

    ProtocolFrame frame;
    frame.isValid = true;
    frame.sequence = 2;
    frame.timestampMs = 1020;
    frame.values = QVector<double>{0.25, 0.35, 0.45};
    frame.rawFrame = "$DATA,2,1020,0.25,0.35,0.45*13";

    recorder.append(frame);
    recorder.stop();

    QFile file(filePath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString content = QString::fromUtf8(file.readAll());

    QVERIFY(content.contains("host_time,sequence,device_time_ms,ch1,ch2,ch3,raw_frame"));
    QVERIFY(content.contains(",2,1020,0.25,0.35,0.45,"));
    QVERIFY(content.contains("\"$DATA,2,1020,0.25,0.35,0.45*13\""));
}

void CoreTest::csvRecorder_appendsMultipleFrames()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    CsvRecorder recorder;
    const QString filePath = tempDir.filePath("multiple_frames.csv");
    QVERIFY(recorder.start(filePath));

    ProtocolFrame frame1;
    frame1.isValid = true;
    frame1.sequence = 1;
    frame1.timestampMs = 1000;
    frame1.values = QVector<double>{0.20, 0.30, 0.40};
    frame1.rawFrame = "$DATA,1,1000,0.20,0.30,0.40*17";

    ProtocolFrame frame2;
    frame2.isValid = true;
    frame2.sequence = 2;
    frame2.timestampMs = 1020;
    frame2.values = QVector<double>{0.25, 0.35, 0.45};
    frame2.rawFrame = "$DATA,2,1020,0.25,0.35,0.45*13";

    recorder.append(frame1);
    recorder.append(frame2);
    recorder.stop();

    QFile file(filePath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString content = QString::fromUtf8(file.readAll());

    const QStringList lines = content.split('\n', Qt::SkipEmptyParts);
    QCOMPARE(lines.size(), 3); // 1 行表头 + 2 行数据

    QVERIFY(lines[1].contains(",1,1000,0.2,0.3,0.4,")
         || lines[1].contains(",1,1000,0.20,0.30,0.40,"));
    QVERIFY(lines[2].contains(",2,1020,0.25,0.35,0.45,"));
}

void CoreTest::csvRecorder_ignoresAppendsAfterStop()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    CsvRecorder recorder;
    const QString filePath = tempDir.filePath("stop_ignore.csv");
    QVERIFY(recorder.start(filePath));

    ProtocolFrame frame;
    frame.isValid = true;
    frame.sequence = 1;
    frame.timestampMs = 1000;
    frame.values = QVector<double>{0.20, 0.30, 0.40};
    frame.rawFrame = "$DATA,1,1000,0.20,0.30,0.40*17";

    recorder.append(frame);
    recorder.stop();
    recorder.append(frame);

    QFile file(filePath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString content = QString::fromUtf8(file.readAll());

    const QStringList lines = content.split('\n', Qt::SkipEmptyParts);
    QCOMPARE(lines.size(), 2); // 1 行表头 + 1 行数据
}

void CoreTest::csvRecorder_escapesRawFrameForCsv()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    CsvRecorder recorder;
    const QString filePath = tempDir.filePath("escape.csv");
    QVERIFY(recorder.start(filePath));

    ProtocolFrame frame;
    frame.isValid = true;
    frame.sequence = 9;
    frame.timestampMs = 9999;
    frame.values = QVector<double>{1.0, 2.0, 3.0};
    frame.rawFrame = "$DATA,9,9999,\"quoted\",2.0,3.0*AA";

    recorder.append(frame);
    recorder.stop();

    QFile file(filePath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString content = QString::fromUtf8(file.readAll());

    QVERIFY(content.contains("\"$DATA,9,9999,\"\"quoted\"\",2.0,3.0*AA\""));
}


QTEST_MAIN(CoreTest)
#include "release/tst_core.moc"
