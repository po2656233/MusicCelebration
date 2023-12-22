/*
 * Copyright (c) 2018-2023 WangBin <wbsecg1 at gmail.com>
 * MDK SDK with QOpenGLWidget example
 */
#ifndef QMDKPlayer_H
#define QMDKPlayer_H

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
    void seek(qint64 ms);

    void addRenderer(QObject* vo = nullptr, int w = -1, int h = -1);
    void renderVideo(QObject* vo = nullptr);

    void destroyGLContext(QObject* vo);
    void setROI(QObject* vo, const float* videoRoi, const float* viewportRoi = nullptr);

    QString currentMedia()const;
    qint64 position() const;
    qint64 duration() const;
    qint64 startTime() const;
public slots:
    void play();
    void pause(bool value = true);
    void stop();

private:
    std::unique_ptr<mdk::Player> player_;
};

#endif // QMDKPlayer_H
