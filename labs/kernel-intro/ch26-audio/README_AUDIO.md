## Userspace API

Userspace API - Share headers between the userspace and kernel.

- include/uapi/sound/asound.h

ALSA PCM plugins
https://www.alsa-project.org/alsa-doc/alsa-lib/pcm_plugins.html

Alexandre Belloni
https://github.com/thesofproject/sof


```sh
 $ sudo cat /proc/asound/card0/pcm3p/info
card: 0
device: 3
subdevice: 0
stream: PLAYBACK
id: HDMI 0
name: HDMI 0
subname: subdevice #0
class: 0
subclass: 0
subdevices_count: 1
subdevices_avail: 1

speaker-test -c 2 -t sine -f 800
speaker-test -c 2 -D inverted
```

## Sound System

In a sound system context, a channel refers to a single, separate, and distinct stream of audio information within
a recording, playback, or mixing process. It represents a specific pathway for sound to travel from a source to
a specific loudspeaker

Here is a breakdown of what a channel means in different scenarios:

Playback Systems (Stereo/Surround): Channels correspond to the number of speakers producing sound.
A 1-channel system is mono. A 2-channel system is stereo (left and right). In surround sound (e.g., 5.1),
the "5" represents five main ear-level channels (left, right, center, left-surround, right-surround).

Amplifiers: An amplifier channel is a distinct source of power designed to drive one or more speakers.
A 4-channel amp can power four separate speakers independently.

Subwoofers (.1): The decimal number in systems (e.g., the ".1" in 5.1) represents a dedicated channel
for low-frequency effects (LFE) that handles deep bass.

Recording/Mixing: A channel represents a single input (e.g., a microphone, a guitar) on a mixing board,
allowing it to be treated independently—adjusted for volume, equalization, or effects—before being mixed
into a final stereo or surround file.

Key Takeaways:

Number = Location: More channels generally mean a more immersive or directional soundscape.

Channels vs. Speakers: While often used interchangeably, a single channel can sometimes power
multiple speakers, or one physical speaker box can contain multiple channels (like a soundbar).

Standard Types: Mono (1.0), Stereo (2.0), 2.1 (Stereo + Sub), 5.1 (Surround), 7.1 (Advanced Surround),
and 5.1.2/7.1.4 (Object-based/Dolby Atmos with height).

## Simple Card

The DT bindings are documented in
Documentation/devicetree/bindings/sound/simple-card.yaml

The driver handling it is sound/soc/generic/simple-card.c

The values of the different clocks can be found in:

```
sudo cat /sys/kernel/debug/clk/clk_summary
```

## Routing

The list of physical audio connections listed in the dts is called
routing.

The steps to define the routing are:

1. Define the connectors

```
simple-audio-card,widgets = {
	"Line", "Line0",
	"Line", "Line1",
	"Headphone", "Headphone Jack",
};
```

2. Define the connections

```
simple-audio-card,routing = {
	"AIN0", "Line0",
	"AIN1", "Line0",
	"AIN2", "Line1",
	"AIN3", "Line1",
	"Headphone Jack", "HPOUTL",
	"Headphone Jack", "HPOUTR",
};
```

## Drivers

The audio hardware consists of:

- Dynamic Audio Interface (DAI)
- Codecs
- Aux devices
- Amplifiers
- SoC


```
          SoC
    ----------------             --------------           -------------
    |               |    BCLK    |            |           |           |
    |         DAI   |------------|            |-----------| Amplifier |
    |               |     Tx     |            |           |           |
    |               |------------|   CODEC    |           -------------
    |               |     Rx     |            |
    |               |------------|            |
    |               |            --------------
    |      SPI/I2C  |
    ----------------
```

Each of these HW components have a driver to control them.

The Machine Driver in ALSA (specifically within the ASoC—ALSA System on Chip—framework)
is the "glue" code that integrates specific audio hardware components into a functional
sound card. It connects the Platform driver (DMA/CPU interface) and the Codec driver (audio converters),
managing board-specific details like clocking, GPIOs, audio routing, and jacks. It defines the
final audio path and registers the sound card with the kernel.

Detailed Context
In Linux audio, the ASoC architecture breaks down a complex audio system into three distinct parts.
The Machine Driver is the top layer that ties them all together:

`Codec Driver`: Platform-independent code for the audio codec itself (controls, audio paths, I/O).

`DAI Driver`: Driver for the Digital Audio Interface and it is part of the Codec Driver.

`Platform Driver`: Platform-specific code for the audio DMA engine and digital audio interfaces
(like I2S, AC97, or PCM).

Machine Driver (The Glue)

`Integration`: It tells the kernel how the Codec and Platform components are wired together on a
specific circuit board.

`Board-Specific Logic`: It handles tasks unique to the board, such as toggling an external
amplifier on/off when playback starts or stops.

`Hardware Setup`: It defines and manages physical resources like clock sources, voltage regulators,
interrupts, and physical jack detection.

`Card Registration`: It registers the resulting sound card with the ALSA subsystem,
allowing applications to see and use it as a complete audio device.

Without a machine driver, the kernel would see the individual components
(the codec and the processor's audio interface) but would not know how they connect to
form an actual, usable sound card

The machine driver registers a struct snd_soc_card.

See
include/sound/soc.h

```c
int snd_soc_register_card(struct snd_soc_card *card);
int snd_soc_unregister_card(struct snd_soc_card *card);
int devm_snd_soc_register_card(struct device *dev, struct snd_soc_card *card);
[...]

/* SoC card */
struct snd_soc_card {
	const char *name;
	const char *long_name;
	const char *driver_name;
	struct device *dev;
	struct snd_card *snd_card;
	[...]

	/* CPU <--> Codec DAI links */
	struct snd_soc_dai_link *dai_link; /* predefined links only */
	int num_links; /* predefined links only */
	struct list_head dai_link_list; /* all links */
	int num_dai_links;
	[...]
};
```

`struct snd_soc_dai_link` is used to create the link between the CPU DAI and the
codec DAI.


```c
struct snd_soc_dai_link {
	/* config - must be set by machine driver */
	const char *name; /* Codec name */
	const char *stream_name; /* Stream name */

	struct snd_soc_dai_link_component *cpus;
	unsigned int num_cpus;

	struct snd_soc_dai_link_component *codecs;
	unsigned int num_codecs;
	unsigned int dai_fmt; /* format to set on init */
    ...
};
```

A configurations of a routing example is below:

```c
sound {
	compatible = "atmel,asoc-wm8904";
	pinctrl-names = "default";
	pinctrl-0 = <&pinctrl_pck0_as_mck>;
	atmel,model = "wm8904 @ AT91SAM9N12EK";
	atmel,audio-routing =
		"Headphone Jack", "HPOUTL",
		"Headphone Jack", "HPOUTR",
		"IN2L", "Line In Jack",
		"IN2R", "Line In Jack",
		"Mic", "MICBIAS",
		"IN1L", "Mic";

	atmel,ssc-controller = <&ssc0>;
	atmel,audio-codec = <&wm8904>;
};
```

The different sound system operations that need to be implemented
are described below:

```c
/* SoC audio ops */
struct snd_soc_ops {
	int (*startup)(struct snd_pcm_substream *);
	void (*shutdown)(struct snd_pcm_substream *);
    int (*hw_params)(struct snd_pcm_substream *, struct snd_pcm_hw_params *);
	int (*hw_free)(struct snd_pcm_substream *);
	int (*prepare)(struct snd_pcm_substream *);
	int (*trigger)(struct snd_pcm_substream *, int);
};
```

`hw_params()` is called when setting up the audio stream.

The `struct snd_pcm_hw_params` is used to get the rate, channels,
audio format, bit depth, bit clock, etc.

```c
struct snd_pcm_hw_params {
	unsigned int flags;
	struct snd_mask masks[SNDRV_PCM_HW_PARAM_LAST_MASK -
			       SNDRV_PCM_HW_PARAM_FIRST_MASK + 1];
	struct snd_mask mres[5];	/* reserved masks */
	struct snd_interval intervals[SNDRV_PCM_HW_PARAM_LAST_INTERVAL -
				        SNDRV_PCM_HW_PARAM_FIRST_INTERVAL + 1];
	struct snd_interval ires[9];	/* reserved intervals */
	unsigned int rmask;		/* W: requested masks */
	unsigned int cmask;		/* R: changed masks */
	unsigned int info;		/* R: Info flags for returned setup */
	unsigned int msbits;		/* R: used most significant bits (in sample bit-width) */
	unsigned int rate_num;		/* R: rate numerator */
	unsigned int rate_den;		/* R: rate denominator */
	snd_pcm_uframes_t fifo_size;	/* R: chip FIFO size in frames */
	unsigned char sync[16];		/* R: synchronization ID (perfect sync - one clock source) */
	unsigned char reserved[48];	/* reserved for future */
};
```

The function to extract the rate, channels, audio format, bit depth,
bit clock, etc. from `struct snd_pcm_hw_params` are defined
`include/linux/sound/pcm`. For example:

```c
/**
 * params_channels - Get the number of channels from the hw params
 * @p: hw params
 *
 * Return: the number of channels
 */
static inline unsigned int params_channels(const struct snd_pcm_hw_params *p)
{
	return hw_param_interval_c(p, SNDRV_PCM_HW_PARAM_CHANNELS)->min;
}

/**
 * params_rate - Get the sample rate from the hw params
 * @p: hw params
 *
 * Return: the sample rate
 */
static inline unsigned int params_rate(const struct snd_pcm_hw_params *p)
{
	return hw_param_interval_c(p, SNDRV_PCM_HW_PARAM_RATE)->min;
}
```

### CODEC driver

The CODEC driver registers a `struct snd_soc_component_driver` and
`struct snd_soc_dai_driver`.

linux/include/soc.h

```c
int snd_soc_register_component(struct device *dev,
			 const struct snd_soc_component_driver *component_driver,
			 struct snd_soc_dai_driver *dai_drv, int num_dai);
int devm_snd_soc_register_component(struct device *dev,
			 const struct snd_soc_component_driver *component_driver,
			 struct snd_soc_dai_driver *dai_drv, int num_dai);
```

The `struct snd_soc_component_driver` represents the `CODEC` hardware:

```c
struct snd_soc_component_driver {
	const char *name;

	/* Default control and setup, added after probe() is run */

	/* *controls is an array of controls (volume, mixing, muxing switches)
	    available on the CODEC. */
	const struct snd_kcontrol_new *controls;
	unsigned int num_controls;

	/* *dapm_widgets is an array of power management controls so ASoC can power
	   down the routes that are not currently use. */
	const struct snd_soc_dapm_widget *dapm_widgets;
	unsigned int num_dapm_widgets;

	/* *dapm_routes is an array describing the routes. */
	const struct snd_soc_dapm_route *dapm_routes;
	unsigned int num_dapm_routes;

	int (*probe)(struct snd_soc_component *component);
	void (*remove)(struct snd_soc_component *component);
	int (*suspend)(struct snd_soc_component *component);
	int (*resume)(struct snd_soc_component *component);

	unsigned int (*read)(struct snd_soc_component *component,
			     unsigned int reg);
	int (*write)(struct snd_soc_component *component,
		     unsigned int reg, unsigned int val);

	.....
};
```

A CODEC can have many knobs that can be configured. The knobs are exposed
through this interface:

```c
struct snd_kcontrol_new {
	snd_ctl_elem_iface_t iface;	/* interface identifier */
	unsigned int device;		/* device/client number */
	unsigned int subdevice;		/* subdevice (substream) number */
	const char *name;		/* ASCII name of item */
	unsigned int index;		/* index of item */
	unsigned int access;		/* access rights */
	unsigned int count;		/* count of same elements */
	snd_kcontrol_info_t *info;
	snd_kcontrol_get_t *get;
	snd_kcontrol_put_t *put;
	union {
		snd_kcontrol_tlv_rw_t *c;
		const unsigned int *p;
	} tlv;
	unsigned long private_value;
};
```

The `snd_soc_dai_driver` represents the `Digital Audio Interface Driver`:

```c
/*
 * Digital Audio Interface Driver.
 *
 * Describes the Digital Audio Interface in terms of its ALSA, DAI and AC97
 * operations and capabilities. Codec and platform drivers will register this
 * structure for every DAI they have.
 *
 * This structure covers the clocking, formating and ALSA operations for each
 * interface.
 */
struct snd_soc_dai_driver {
	/* DAI description */
	const char *name;
	unsigned int id;
	unsigned int base;
	struct snd_soc_dobj dobj;
	const struct of_phandle_args *dai_args;

	/* ops */
	const struct snd_soc_dai_ops *ops;
	const struct snd_soc_cdai_ops *cops;

	/* DAI capabilities */
	struct snd_soc_pcm_stream capture;
	struct snd_soc_pcm_stream playback;
	unsigned int symmetric_rate:1;
	unsigned int symmetric_channels:1;
	unsigned int symmetric_sample_bits:1;
};
```

The operations performed by the DAI are defined in `struct snd_soc_dai_ops`:

```C
struct snd_soc_dai_ops {
	/* DAI driver callbacks */
	int (*probe)(struct snd_soc_dai *dai);
	int (*remove)(struct snd_soc_dai *dai);
	/* compress dai */
	int (*compress_new)(struct snd_soc_pcm_runtime *rtd);
	/* Optional Callback used at pcm creation*/
	int (*pcm_new)(struct snd_soc_pcm_runtime *rtd,
		       struct snd_soc_dai *dai);

	/*
	 * DAI format configuration
	 * Called by soc_card drivers, normally in their hw_params.
	 */
	int (*set_fmt)(struct snd_soc_dai *dai, unsigned int fmt);
	int (*xlate_tdm_slot_mask)(unsigned int slots,
		unsigned int *tx_mask, unsigned int *rx_mask);
	int (*set_tdm_slot)(struct snd_soc_dai *dai,
		unsigned int tx_mask, unsigned int rx_mask,
		int slots, int slot_width);
	int (*set_channel_map)(struct snd_soc_dai *dai,
		unsigned int tx_num, const unsigned int *tx_slot,
		unsigned int rx_num, const unsigned int *rx_slot);
	int (*get_channel_map)(const struct snd_soc_dai *dai,
			unsigned int *tx_num, unsigned int *tx_slot,
			unsigned int *rx_num, unsigned int *rx_slot);
	int (*set_tristate)(struct snd_soc_dai *dai, int tristate);

	int (*set_stream)(struct snd_soc_dai *dai,
			  void *stream, int direction);
	void *(*get_stream)(struct snd_soc_dai *dai, int direction);

	/*
	 * DAI digital mute - optional.
	 * Called by soc-core to minimise any pops.
	 */
	int (*mute_stream)(struct snd_soc_dai *dai, int mute, int stream);

	/*
	 * ALSA PCM audio operations - all optional.
	 * Called by soc-core during audio PCM operations.
	 */
	int (*startup)(struct snd_pcm_substream *,
		struct snd_soc_dai *);
	void (*shutdown)(struct snd_pcm_substream *,
		struct snd_soc_dai *);
	int (*hw_params)(struct snd_pcm_substream *,
		struct snd_pcm_hw_params *, struct snd_soc_dai *);
	int (*hw_free)(struct snd_pcm_substream *,
		struct snd_soc_dai *);
	int (*prepare)(struct snd_pcm_substream *,
		struct snd_soc_dai *);
};
```

The capture/playback information is defined by the PCM (Pulse Code Modulation)
format. The `struct snd_soc_pcm_stream` contains the specific format:

```C
/* SoC PCM stream information */
struct snd_soc_pcm_stream {
	const char *stream_name;
	u64 formats;			/* SNDRV_PCM_FMTBIT_* */
	u32 subformats;			/* for S32_LE format, SNDRV_PCM_SUBFMTBIT_* */
	unsigned int rates;		/* SNDRV_PCM_RATE_* */
	unsigned int rate_min;		/* min rate */
	unsigned int rate_max;		/* max rate */
	unsigned int channels_min;	/* min channels */
	unsigned int channels_max;	/* max channels */
	unsigned int sig_bits;		/* number of bits of content */
};
```

- `DAI links` describes how the CODEC and CPU are connected.
- `DAPM widgets` represents analog audio endpoints.
- `Routing` describes how endpoints are connected.

## regmap

-----------------------
Playback and capture
-----------------------

speaker-test
aplay
arecord

------------------------
Troubleshooting
------------------------

Check how long the sound is played.

------------------------------
Enable events in the kernel
------------------------------

```sh
cd /sys/kernel/debug/tracing
wc -l available_events
echo reg > set_event
```
