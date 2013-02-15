CC ?= gcc

TCS = utc_media_camera_attr \
	utc_media_camera_lifecycle \
	utc_media_camera_setting \
	utc_media_camera_working \


PKGS = capi-media-camera dlog glib-2.0 gthread-2.0

LDFLAGS = `pkg-config --libs $(PKGS)`
LDFLAGS += $(TET_ROOT)/lib/tet3/tcm_s.o
LDFLAGS += -L$(TET_ROOT)/lib/tet3 -ltcm_s
LDFLAGS += -L$(TET_ROOT)/lib/tet3 -lapi_s

CFLAGS = -I. `pkg-config --cflags $(PKGS)`
CFLAGS += -I$(TET_ROOT)/inc/tet3
CFLAGS += -Wall -Werror

all: $(TCS)


%: %.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)


clean:
	rm -f $(TCS)
