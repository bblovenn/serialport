#include <QtTest>

#include "core/ringbuffer.h"
#include "core/samplepack.h"

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

QTEST_MAIN(CoreTest)
#include "tst_core.moc"
