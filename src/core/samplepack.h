#ifndef SAMPLEPACK_H
#define SAMPLEPACK_H

#include <QVector>


class SamplePack
{
public:
    SamplePack();
    explicit SamplePack(const QVector<double>& values); 
    int channelCount() const; 
    double value(int channelIndex) const;
    QVector<double> values() const;
    bool isEmpty() const;

private:
    QVector<double> m_values;
};

#endif // SAMPLEPACK_H
