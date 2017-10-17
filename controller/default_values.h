#ifndef CONFIG_H
#define CONFIG_H

#define APPLICATION_VERSION       "0.1"
#define APPLICATION_NAME          "LightController"
#define APPLICATION_AUTHOR        "Roy Bakker"
#define APPLICATION_AUTHOR_EMAIL  "roy@roybakker.nl"
#define APPLICATION_AUTHOR_URL    "www.roybakker.nl"
#define APPLICATION_COMPANY       "roybakker.nl"
#define APPLICATION_YEAR          "2015"
#define APPLICATION_URL           "http://github.com/bakkerr/lightcontroller"

#define DEFAULT_SAVE_NAME         "default"

#define DEFAULT_IP                "192.168.1.14"

#define UDP_PORT_DEFAULT                8899
#define UDP_RESENDS_DEFAULT                1
#define WIRELESS_RESENDS_DEFAULT          10

#define BRIGHT_VALUE_DEFAULT              19

#define AUDIO_THRESHOLD_DEFAULT           80
#define AUDIO_THRESHOLD_MAX              128
#define AUDIO_THRESHOLD_MIN                0
#define AUDIO_SAMPLES_DEFAULT           5000
#define AUDIO_SAMPLES_MIN               2500
#define AUDIO_SAMPLES_MAX              30000
#define AUDIO_INCOMING_SAMPLES_PER_SEC 44100
#define AUDIO_AVERAGE_SAMPLES             10
#define AUDIO_GRAPH_DISPLAY_SAMPLES    (2*AUDIO_INCOMING_SAMPLES_PER_SEC)/AUDIO_AVERAGE_SAMPLES

#define FADE_VALUE_MS_DEFAULT            250
#define FADE_VALUE_MS_MIN                 50
#define FADE_VALUE_MS_MAX               5000


/* Debug values */
#define ADD_DUMMY_DEVICES


#endif // CONFIG_H
