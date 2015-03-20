#ifndef CONFIG_H
#define CONFIG_H

#define STR_(x) #x
#define STR(x) STR_(x)

#define BRIGHT_VALUE_DEFAULT             (19)

#define AUDIO_THRESHOLD_DEFAULT         (220)
#define AUDIO_THRESHOLD_MAX             (256)
#define AUDIO_THRESHOLD_MIN             (128)
#define AUDIO_SAMPLES_DEFAULT           (400)
#define AUDIO_SAMPLES_MIN               (100)
#define AUDIO_SAMPLES_MAX              (4000)
#define AUDIO_INCOMING_SAMPLES_PER_SEC (2000)
#define AUDIO_GRAPH_DISPLAY_SAMPLES    (5000)
#define AUDIO_GRAPH_UPDATE_SAMPLES      (400)
#define AUDIO_RECORD_COMMAND_DEFAULT   "arecord -t raw -D pulse -f U8 -r " STR(AUDIO_INCOMING_SAMPLES_PER_SEC) #" 2> /dev/null"


#define FADE_VALUE_MS_DEFAULT           (250)
#define FADE_VALUE_MS_MIN                (50)
#define FADE_VALUE_MS_MAX              (5000)


#endif // CONFIG_H
