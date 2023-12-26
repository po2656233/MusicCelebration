/*
 * Copyright (c) 2018-2023 WangBin <wbsecg1 at gmail.com>
 * MDK SDK with QOpenGLWidget example
 */
#ifndef QMDKPlayer_H
#define QMDKPlayer_H

#include "mdk/Player.h"
#include <QObject>
#include <memory>

namespace mdk {
class Player;
}
#ifndef Q_MDK_API
#define Q_MDK_API Q_DECL_IMPORT
#endif
class Q_MDK_API QMDKPlayer : public QObject
{
    Q_OBJECT
public:
    QMDKPlayer(QObject *parent = nullptr);
    ~QMDKPlayer();
    // decoders: "VideoToolbox", "VAAPI", "VDPAU", "D3D11", "DXVA", "NVDEC", "CUDA", "CedarX"(sunxi), "AMediaCodec", "FFmpeg"
    void setDecoders(const QStringList& dec);
    void setMedia(const QString& url);
    void setVolume(float val);
    void setMuted(bool muted);
    void setPlaybackRate(float value);


    bool isPaused() const;
    bool isPlaying()const;
    bool isStopped() const;
    void seek(qint64 ms);

    void addRenderer(QObject* vo = nullptr, int w = -1, int h = -1);
    void renderVideo(QObject* vo = nullptr);

    void destroyGLContext(QObject* vo);
    void setROI(QObject* vo, const float* videoRoi, const float* viewportRoi = nullptr);

    QString currentMedia()const;
    qint64 position() const;
    qint64 duration() const;
    qint64 startTime() const;
    void setLoop(int count);
    void rotate(int degree, QObject *render);
    void setAspect(float value, QObject *render);
    void setBackgroundColor(float r, float g, float b, float a, QObject *render);
    QSize getSize(int stream);
    float playbackRate();
    void record(const QString &fileName, const QString &format);
    void readMetaData();
    void clear(QObject *render);
    void setVideoSurfaceSize(int width, int height, QObject *render);
    void setVideoTrack(int track);
    void setAudioTrack(int track);
    void readTrackInfo(QList<int> &audioTracks, QList<int> &videoTracks);
    void setFilter(const QString &filter);
    void setProperty(const std::string &key, const std::string &value);

signals:
    void signalStateChanged(mdk::State state);
    void signalEventChanged(mdk::MediaEvent e);
    void signalMediaStatusChanged(mdk::MediaStatus state);
    void signalReceiveMetaData(QString, QString, QString, QString);

public slots:
    void play();
    void pause(bool value = true);
    void stop();
private:
    std::unique_ptr<mdk::Player> player_;
};

#endif // QMDKPlayer_H
