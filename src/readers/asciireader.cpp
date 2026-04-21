#include "asciireader.h"

#include <QIODevice>
#include <QVector>

// 构造函数
AsciiReader::AsciiReader(QObject* parent)
    : AbstractReader(parent)
    , m_device(nullptr)  // 设备初始为空
    , m_running(false)    // 运行标志初始为false
{
}

// 设置数据源设备（通常是串口）
void AsciiReader::setDevice(QIODevice* device)
{
    // 断开旧设备的连接
    if (m_device) {
        disconnect(m_device, nullptr, this, nullptr);
    }

    m_device = device;  // 保存新设备

    // 连接新设备的readyRead信号
    if (m_device) {
        connect(m_device, &QIODevice::readyRead, this, &AsciiReader::readAvailableData);
    }
}

// 启动数据接收
void AsciiReader::start()
{
    m_running = true;  // 设置运行标志
}

// 停止数据接收
void AsciiReader::stop()
{
    m_running = false;   // 清除运行标志
    m_buffer.clear();   // 清空缓冲区
}

// 读取设备可用数据
void AsciiReader::readAvailableData()
{
    // 未启动或无设备则直接返回
    if (!m_running || !m_device) {
        return;
    }

    // 读取设备中的所有数据到缓冲区
    m_buffer.append(m_device->readAll());

    // 查找换行符，逐行处理
    int newlineIndex = m_buffer.indexOf('\n');
    while (newlineIndex >= 0) {
        // 提取一行并去除首尾空白
        const QByteArray line = m_buffer.left(newlineIndex).trimmed();
        // 从缓冲区移除已处理的数据
        m_buffer.remove(0, newlineIndex + 1);

        // 非空行则解析
        if (!line.isEmpty()) {
            parseLine(line);
        }

        // 查找下一个换行符
        newlineIndex = m_buffer.indexOf('\n');
    }
}

// 解析一行CSV数据
void AsciiReader::parseLine(const QByteArray& line)
{
    QVector<double> values;  // 存储解析出的数值
    const QList<QByteArray> parts = line.split(',');  // 按逗号分割

    // 逐个解析逗号分隔的数值
    for (const QByteArray& part : parts) {
        bool ok = false;
        const double value = part.trimmed().toDouble(&ok);  // 尝试转为double
        if (!ok) {
            return;  // 解析失败则丢弃整行
        }

        values.append(value);  // 添加有效数值
    }

    // 有有效数值则发送信号
    if (!values.isEmpty()) {
        emit samplesReady(SamplePack(values));
    }
}
