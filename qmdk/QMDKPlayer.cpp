/*
 * Copyright (c) 2018-2023 WangBin <wbsecg1 at gmail.com>
 * MDK SDK with QOpenGLWindow example
 */
#include "QMDKPlayer.h"
#include <QCoreApplication>
#include <QStringList>
#include <QtDebug>
#include <QRegion>
#include <QVariant>
// #include<QImage>
#include <QThread>
#include "mdk/Player.h"



using namespace MDK_NS;
QMDKPlayer::QMDKPlayer(QObject *parent)
    : QObject(parent)
    , player_(new Player())
{
    player_->setRenderCallback([](void* vo_opaque){
        auto vo = reinterpret_cast<QObject*>(vo_opaque);
        if (!vo->isWidgetType()) { // isWidgetType() is fastest, and no need to include <QWidget>
            if (vo->isWindowType())
                QCoreApplication::instance()->postEvent(vo, new QEvent(QEvent::UpdateRequest));
            return;
        }
        QMetaObject::invokeMethod(vo, "update", Qt::QueuedConnection);
#if 0
        class QUpdateLaterEvent final : public QEvent {
        public:
            explicit QUpdateLaterEvent(const QRegion& paintRegion)
                : QEvent(UpdateLater), m_region(paintRegion)
            {}
            ~QUpdateLaterEvent() {}
            inline const QRegion &region() const { return m_region; }
        protected:
            QRegion m_region;
        };
        QCoreApplication::instance()->postEvent(vo, new QUpdateLaterEvent(QRegion(0, 0, vo->property("width").toInt(), vo->property("height").toInt())));
#endif
    });
    // player_->setVideoDecoders({"VT", "VAAPI", "MFT:d3d=11", "DXVA", "MMAL", "AMediaCodec:java=1:copy=0:surface=1:async=0", "FFmpeg"});
    SetGlobalOption("plugins", "mdk-r3d:mdk-braw");
    qRegisterMetaType<mdk::State>("mdk::State");
    qRegisterMetaType<mdk::MediaStatus>("mdk::MediaStatus");
    //播放状态变化
    player_->onStateChanged([this](mdk::State state) {
        emit this->signalStateChanged(state);
    });

    //媒体状态变化
    player_->onMediaStatus([this](mdk::MediaStatus oldValue, mdk::MediaStatus newValue) {
        emit this->signalMediaStatusChanged(newValue);
        return false;
    });


    //各种事件触发
    // player_->onEvent([this](const mdk::MediaEvent & e) {
    //     emit signalEventChanged(e);
    //     return true;
    // });

    //获取状态
    //  MediaStatus a=player_->mediaStatus();

}

QMDKPlayer::~QMDKPlayer()=default;
void QMDKPlayer::setDecoders(const QStringList &dec)
{
    if(dec.isEmpty()){
        player_->setDecoders(MediaType::Video, {"MFT:d3d=11", "D3D11", "CUDA", "hap", "FFmpeg", "dav1d"});
        return;
    }
    std::vector<std::string> v;
    foreach (QString d, dec) {
        v.push_back(d.toStdString());
    }

    player_->setDecoders(MediaType::Video, v);
}

void QMDKPlayer::setMedia(const QString &url)
{
    player_->setMedia(url.toUtf8().constData());
    player_->waitFor(mdk::State::Stopped);
    player_->prepare(0, [this](int64_t  v, bool *isOk) {
        QMetaObject::invokeMethod(parent(), "readMediaInfo");
        qDebug()<<"setMedia"<<v<<*isOk;
        return true;
    });
    // QThread::usleep(800);
    QThread::msleep(100);
}
void QMDKPlayer::setFilter(const QString &filter)
{
    player_->setProperty("video.avfilter", filter.toStdString());
}
void QMDKPlayer::setProperty(const std::string &key, const std::string &value)
{
    player_->setProperty(key, value);
}

void QMDKPlayer::setVolume(float val)
{
    player_->setVolume(val/15);
}

void QMDKPlayer::setMuted(bool muted)
{
    player_->setMute(muted);
}

void QMDKPlayer::setPlaybackRate(float value)
{
    //设置播放器播放速度  正常速度是1.0
    player_->setPlaybackRate(value);
}

void QMDKPlayer::play()
{

    player_->set(State::Playing);
}

void QMDKPlayer::pause(bool value)
{
    if (value)
        player_->set(State::Paused);
    else
        play();
}

void QMDKPlayer::stop()
{
    player_->set(State::Stopped);
}

bool QMDKPlayer::isPaused() const
{
    return player_->state() == State::Paused;
}

bool QMDKPlayer::isPlaying() const
{
    return player_->state() == State::Playing;
}
bool QMDKPlayer::isStopped() const
{
    return player_->state() == State::Stopped;
}

void QMDKPlayer::seek(qint64 ms)
{
    player_->seek(ms);
}

qint64 QMDKPlayer::position() const
{
    return player_->position();
}

qint64 QMDKPlayer::duration() const
{
    mdk::MediaInfo mediaInfo = player_->mediaInfo();

    std::vector<mdk::AudioStreamInfo> audioTrackInfo = mediaInfo.audio;
    foreach (mdk::AudioStreamInfo info, audioTrackInfo) {
        return info.duration;
    }

    std::vector<mdk::VideoStreamInfo> videoTrackInfo = mediaInfo.video;
    foreach (mdk::VideoStreamInfo info, videoTrackInfo) {
        return info.duration;
    }
    return mediaInfo.duration;
}

qint64 QMDKPlayer::startTime() const
{
    mdk::MediaInfo mediaInfo = player_->mediaInfo();

    std::vector<mdk::AudioStreamInfo> audioTrackInfo = mediaInfo.audio;
    foreach (mdk::AudioStreamInfo info, audioTrackInfo) {
        qDebug()<<"start_time1 "<<info.start_time;
        return info.start_time;
    }

    std::vector<mdk::VideoStreamInfo> videoTrackInfo = mediaInfo.video;
    foreach (mdk::VideoStreamInfo info, videoTrackInfo) {
        qDebug()<<"start_time2 "<<info.start_time;
        return info.start_time;
    }
    qDebug()<<"start_time3 "<<mediaInfo.start_time;
    return mediaInfo.start_time;
}

void QMDKPlayer::addRenderer(QObject* vo, int w, int h)
{
    if (w <= 0)
        w = vo->property("width").toInt() * qMax(vo->property("devicePixelRatio").toInt(), 1);
    if (h <= 0)
        h = vo->property("height").toInt() * qMax(vo->property("devicePixelRatio").toInt(), 1);
    player_->setVideoSurfaceSize(w, h, vo); // call update cb
    connect(vo, &QObject::destroyed, this, [this](QObject* obj){
            player_->setVideoSurfaceSize(-1, -1, obj); // remove vo
        }, Qt::DirectConnection);
}

void QMDKPlayer::renderVideo(QObject* vo)
{
    player_->renderVideo(vo);
}

void QMDKPlayer::destroyGLContext(QObject* vo)
{
    player_->setVideoSurfaceSize(-1, -1, vo); // it's better to cleanup gl renderer resources
}


void QMDKPlayer::setROI(QObject* vo, const float* videoRoi, const float* viewportRoi)
{
    player_->setPointMap(videoRoi, viewportRoi, 2, vo);

}

QString QMDKPlayer::currentMedia() const
{
    return player_.get()->url();
}
void QMDKPlayer::setLoop(int count)
{
    player_->setLoop(count);
}

void QMDKPlayer::rotate(int degree, QObject *render)
{
    player_->rotate(degree, render);
}

void QMDKPlayer::setAspect(float value, QObject *render)
{
    player_->setAspectRatio(value, render);
}

void QMDKPlayer::setBackgroundColor(float r, float g, float b, float a, QObject *render)
{
    player_->setBackgroundColor(r, g, b, a, render);
}

QSize QMDKPlayer::getSize(int stream)
{
    QSize size;
    std::vector<mdk::VideoStreamInfo> video = player_->mediaInfo().video;
    if (video.size() > stream) {
        mdk::VideoCodecParameters para = video.at(stream).codec;
        size = QSize(para.width, para.height);
    }

    return size;
}

float QMDKPlayer::playbackRate()
{
    return player_->playbackRate();
}
void QMDKPlayer::record(const QString &fileName, const QString &format)
{
    player_->record(fileName.toUtf8().constData(), format.toUtf8().constData());
}
// void QMDKPlayer::snapshot(int rotate, QObject *render)
// {
//     mdk::Player::SnapshotRequest sr{};
//     player_->snapshot(&sr, [this, rotate](mdk::Player::SnapshotRequest * sr2, double) {
//             QImage image = QImage(sr2->data, sr2->width, sr2->height, Format_RGB);
//             //如果有旋转角度先要旋转
//             VideoHelper::rotateImage(rotate, image);
//             emit imageCaptured(image.copy());
//             return std::string();
//         }, render);
// }
void QMDKPlayer::readMetaData()
{
    //标题/艺术家/专辑/专辑封面
    QString title, artist, album;
    mdk::MediaInfo mediaInfo = player_->mediaInfo();
    std::unordered_map<std::string, std::string> metadata = mediaInfo.metadata;
    for (auto i = metadata.begin(); i != metadata.end(); i++) {
        QString key = QString::fromStdString(i->first);
        QString value = QString::fromStdString(i->second);
        if (key == "title") {
            title = value;
        } else if (key == "artist") {
            artist = value;
        } else if (key == "album") {
            album = value;
        }
    }

    QString format = mediaInfo.format;
    emit signalReceiveMetaData(format, title, artist, album);
}

// void QMDKPlayer::readAudioInfo(int index)
// {
//     std::vector<mdk::AudioStreamInfo> audios = player_->mediaInfo().audio;
//     if (index >= 0 && audios.size() > index) {
//         mdk::AudioStreamInfo audio = audios.at(index);
//         mdk::AudioCodecParameters para = audio.codec;
//         emit receiveAudioInfo(audio.index, para.sample_rate, para.channels, para.profile, para.bit_rate, para.codec);
//     }
// }

// void QMDKPlayer::readVideoInfo(int index)
// {
//     std::vector<mdk::VideoStreamInfo> videos = player_->mediaInfo().video;
//     if (index >= 0 && videos.size() > index) {
//         mdk::VideoStreamInfo video = videos.at(index);
//         mdk::VideoCodecParameters para = video.codec;
//         //取出封面
//         QImage image;
//         if (video.image_size > 0) {
//             image = QImage::fromData(video.image_data, video.image_size);
//         }
//         emit receiveVideoInfo(video.index, para.width, para.height, para.frame_rate, video.rotation, para.codec, image);
//     }
// }

void QMDKPlayer::readTrackInfo(QList<int> &audioTracks, QList<int> &videoTracks)
{
    mdk::MediaInfo mediaInfo = player_->mediaInfo();

    audioTracks.clear();
    std::vector<mdk::AudioStreamInfo> audioTrackInfo = mediaInfo.audio;
    foreach (mdk::AudioStreamInfo info, audioTrackInfo) {
        audioTracks << info.index;
    }

    videoTracks.clear();
    std::vector<mdk::VideoStreamInfo> videoTrackInfo = mediaInfo.video;
    foreach (mdk::VideoStreamInfo info, videoTrackInfo) {
        videoTracks << info.index;
    }

    //可能获取到的索引是 音频(3, 4, 5) / 视频(0, 1, 2)
    //底层设置节目流按照 0/1/2 这样排列/所以需要强制矫正
    int count = videoTracks.count();
    audioTracks.clear();
    videoTracks.clear();
    for (int i = 0; i < count; ++i) {
        audioTracks << i;
        videoTracks << i;
    }
}

void QMDKPlayer::setAudioTrack(int track)
{
    std::set<int> tracks;
    tracks.insert(track);
    player_->setActiveTracks(mdk::MediaType::Audio, tracks);
}

void QMDKPlayer::setVideoTrack(int track)
{
    std::set<int> tracks;
    tracks.insert(track);
    player_->setActiveTracks(mdk::MediaType::Video, tracks);
}


void QMDKPlayer::setVideoSurfaceSize(int width, int height, QObject *render)
{
    player_->setVideoSurfaceSize(width, height, render);
    connect(render, SIGNAL(destroyed(QObject *)), this, SLOT(clear(QObject *)), Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
}

void QMDKPlayer::clear(QObject *render)
{
    player_->setVideoSurfaceSize(-1, -1, render);
}
