# Raw Audio Media Types

**audio/x-raw**

 - **format**, `G_TYPE_STRING`: Mandatory. The format of the audio samples, see
   the Formats section for a list of valid sample formats.

 - **rate**, `G_TYPE_INT`: Mandatory. The samplerate of the audio

 - **channels**, `G_TYPE_INT`: Mandatory. The number of channels

 - **channel-mask**, `GST_TYPE_BITMASK`: Mandatory for more than 2 channels
   Bitmask of channel positions present. May be omitted for mono and
   stereo. May be set to 0 to denote that the channels are unpositioned.

 - **layout**, `G_TYPE_STRING`: Mandatory. The layout of channels within a
   buffer. Possible values are "interleaved" (for LRLRLRLR) and
   "non-interleaved" (LLLLRRRR)

Use `GstAudioInfo` and related helper API to create and parse raw audio caps.

## Metadata

 - `GstAudioDownmixMeta`: A matrix for downmixing multichannel audio to a
   lower numer of channels.

## Formats

The following values can be used for the format string property.

 - "S8" 8-bit signed PCM audio
 - "U8" 8-bit unsigned PCM audio

 - "S16LE" 16-bit signed PCM audio
 - "S16BE" 16-bit signed PCM audio
 - "U16LE" 16-bit unsigned PCM audio
 - "U16BE" 16-bit unsigned PCM audio

 - "S24\_32LE" 24-bit signed PCM audio packed into 32-bit
 - "S24\_32BE" 24-bit signed PCM audio packed into 32-bit
 - "U24\_32LE" 24-bit unsigned PCM audio packed into 32-bit
 - "U24\_32BE" 24-bit unsigned PCM audio packed into 32-bit

 - "S32LE" 32-bit signed PCM audio
 - "S32BE" 32-bit signed PCM audio
 - "U32LE" 32-bit unsigned PCM audio
 - "U32BE" 32-bit unsigned PCM audio

 - "S24LE" 24-bit signed PCM audio
 - "S24BE" 24-bit signed PCM audio
 - "U24LE" 24-bit unsigned PCM audio
 - "U24BE" 24-bit unsigned PCM audio

 - "S20LE" 20-bit signed PCM audio
 - "S20BE" 20-bit signed PCM audio
 - "U20LE" 20-bit unsigned PCM audio
 - "U20BE" 20-bit unsigned PCM audio

 - "S18LE" 18-bit signed PCM audio
 - "S18BE" 18-bit signed PCM audio
 - "U18LE" 18-bit unsigned PCM audio
 - "U18BE" 18-bit unsigned PCM audio

 - "F32LE" 32-bit floating-point audio
 - "F32BE" 32-bit floating-point audio
 - "F64LE" 64-bit floating-point audio
 - "F64BE" 64-bit floating-point audio

