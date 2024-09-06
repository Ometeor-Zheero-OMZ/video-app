extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <inttypes.h>
}

bool load_frame(const char *filename, int *width_out, int *height_out, unsigned char **data_out)
{
    // Open the file using libavformat
    AVFormatContext *av_format_ctx = avformat_alloc_context();
    if (!av_format_ctx)
    {
        printf("Couldn't create AVFormatContext\n");
        return false;
    }

    if (avformat_open_input(&av_format_ctx, filename, NULL, NULL) != 0)
    {
        printf("Couldn't open video file\n");
        avformat_free_context(av_format_ctx);
        return false;
    }

    // Find the first valid video stream inside the file
    int video_stream_index = -1;
    AVCodecParameters *av_codec_params;
    AVCodec *av_codec;

    for (int i = 0; i < av_format_ctx->nb_streams; ++i)
    {
        av_codec_params = av_format_ctx->streams[i]->codecpar;
        av_codec = avcodec_find_decoder(av_codec_params->codec_id);

        if (!av_codec)
        {
            continue;
        }
        if (av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_stream_index = i;
            break;
        }
    }
    if (video_stream_index == -1)
    {
        printf("Couldn't find valid video stream inside file\n");
        avformat_close_input(&av_format_ctx);
        avformat_free_context(av_format_ctx);
        return false;
    }

    // Set up a codec context for the decoder
    AVCodecContext *av_codec_ctx = avcodec_alloc_context3(av_codec);
    if (!av_codec_ctx)
    {
        printf("Couldn't create AVCodecContext\n");
        avformat_close_input(&av_format_ctx);
        avformat_free_context(av_format_ctx);
        return false;
    }
    if (avcodec_parameters_to_context(av_codec_ctx, av_codec_params) < 0)
    {
        printf("Couldn't initialize AVCodecContext\n");
        avcodec_free_context(&av_codec_ctx);
        avformat_close_input(&av_format_ctx);
        avformat_free_context(av_format_ctx);
        return false;
    }
    if (avcodec_open2(av_codec_ctx, av_codec, NULL) < 0)
    {
        printf("Couldn't open codec\n");
        avcodec_free_context(&av_codec_ctx);
        avformat_close_input(&av_format_ctx);
        avformat_free_context(av_format_ctx);
        return false;
    }

    AVFrame *av_frame = av_frame_alloc();
    if (!av_frame)
    {
        printf("Couldn't allocate AVFrame\n");
        avcodec_free_context(&av_codec_ctx);
        avformat_close_input(&av_format_ctx);
        avformat_free_context(av_format_ctx);
        return false;
    }
    AVPacket *av_packet = av_packet_alloc();
    if (!av_packet)
    {
        printf("Couldn't allocate AVPacket\n");
        avcodec_free_context(&av_codec_ctx);
        avformat_close_input(&av_format_ctx);
        avformat_free_context(av_format_ctx);
        return false;
    }

    int response;
    while (av_read_frame(av_format_ctx, av_packet) >= 0)
    {
        if (av_packet->stream_index != video_stream_index)
        {
            av_packet_unref(av_packet);
            continue;
        }

        char errbuf[AV_ERROR_MAX_STRING_SIZE];
        response = avcodec_send_packet(av_codec_ctx, av_packet);
        if (response < 0)
        {
            av_strerror(response, errbuf, sizeof(errbuf));
            printf("Failed to decode packet: %s\n", errbuf);
            av_packet_free(&av_packet);
            av_frame_free(&av_frame);
            avcodec_free_context(&av_codec_ctx);
            avformat_close_input(&av_format_ctx);
            avformat_free_context(av_format_ctx);
            return false;
        }

        response = avcodec_receive_frame(av_codec_ctx, av_frame);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF)
        {
            av_packet_unref(av_packet);
            continue;
        }
        else if (response < 0)
        {
            av_strerror(response, errbuf, sizeof(errbuf));
            printf("Failed to decode packet: %s\n", errbuf);
            av_packet_free(&av_packet);
            av_frame_free(&av_frame);
            avcodec_free_context(&av_codec_ctx);
            avformat_close_input(&av_format_ctx);
            avformat_free_context(av_format_ctx);
            return false;
        }

        av_packet_unref(av_packet);
        break;
    }

    // Allocate memory for the RGB data (4 bytes per pixel for RGB)
    uint8_t *data = new uint8_t[av_frame->width * av_frame->height * 4];

    // Set up the scaling context for converting YUV to RGB
    SwsContext *sws_scaler_ctx = sws_getContext(av_frame->width,
                                                av_frame->height,
                                                av_codec_ctx->pix_fmt,
                                                av_frame->width,
                                                av_frame->height,
                                                AV_PIX_FMT_RGB24,
                                                SWS_BILINEAR,
                                                NULL,
                                                NULL,
                                                NULL);

    if (!sws_scaler_ctx)
    {
        printf("Couldn't initialize sw scaler\n");
        av_frame_free(&av_frame);
        av_packet_free(&av_packet);
        avcodec_free_context(&av_codec_ctx);
        avformat_close_input(&av_format_ctx);
        avformat_free_context(av_format_ctx);
        delete[] data;
        return false;
    }

    // Set up the destination buffer for scaling
    uint8_t *dest[4] = {data, NULL, NULL, NULL};
    int dest_linesize[4] = {av_frame->width * 4, 0, 0, 0};

    // Scale the image to RGB
    sws_scale(sws_scaler_ctx, av_frame->data, av_frame->linesize, 0, av_frame->height, dest, dest_linesize);
    // Free the scaling context
    sws_freeContext(sws_scaler_ctx);

    // Output the frame data
    *width_out = av_frame->width;
    *height_out = av_frame->height;
    *data_out = data;

    // Clean up
    avformat_close_input(&av_format_ctx);
    avformat_free_context(av_format_ctx);
    av_frame_free(&av_frame);
    av_packet_free(&av_packet);
    avcodec_free_context(&av_codec_ctx);

    return true;
}