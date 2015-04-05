#ifndef CONFIG_H
#define CONFIG_H

#define STR_(x) #x
#define STR(x) STR_(x)

#define MILIGHT_IP_DEFAULT              "192.168.1.16"
#define MILIGHT_PORT_DEFAULT            8899

#define BRIGHT_VALUE_DEFAULT              19

#define AUDIO_THRESHOLD_DEFAULT           80
#define AUDIO_THRESHOLD_MAX              128
#define AUDIO_THRESHOLD_MIN                0
#define AUDIO_SAMPLES_DEFAULT           4096
#define AUDIO_SAMPLES_MIN               2048
#define AUDIO_SAMPLES_MAX              88200
#define AUDIO_INCOMING_SAMPLES_PER_SEC 44100
#define AUDIO_GRAPH_DISPLAY_SAMPLES    88200
#define AUDIO_GRAPH_UPDATE_SAMPLES      4096

#define FADE_VALUE_MS_DEFAULT            250
#define FADE_VALUE_MS_MIN                 50
#define FADE_VALUE_MS_MAX               5000


/* Debug values */
#define ADD_DUMMY_DEVICES



#endif // CONFIG_H
