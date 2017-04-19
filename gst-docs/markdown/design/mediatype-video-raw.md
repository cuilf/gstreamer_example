# Raw Video Media Types

**video/x-raw**

 - **width**, `G_TYPE_INT`: Mandatory. The width of the image in pixels.

 - **height**, `G_TYPE_INT`: Mandatory. The height of the image in pixels

 - **framerate**, `GST_TYPE_FRACTION`: Default 0/1. The framerate of the video,
   0/1 for variable framerate

 - **max-framerate**, `GST_TYPE_FRACTION`: Default as framerate. For variable
   framerates this would be the maximum expected framerate. This
   value is only valid when the framerate is set to "variable" (0/1)

 - **views**, `G_TYPE_INT`: Default 1. The number of views for multiview video.
   Each buffer contains multiple `GstVideoMeta` buffers that describe each
   view. use the frame id to get access to the different views.

 - **interlace-mode**, `G_TYPE_STRING`: Default progressive. The interlace
   mode. The following values are possible:

   - *"progressive"*: all frames are progressive

   - *"interleaved"*: 2 fields are interleaved in one video frame. Extra buffer
     flags describe the field order.

   - *"mixed"*: progressive and interleaved frames, extra buffer flags
     describe the frame and fields.

   - *"fields"*: 2 fields are stored in one buffer. Use the frame ID
     to get access to the required field. For multiview (the
     'views' property > 1) the fields of view N can be found at
     frame ID (N * 2) and (N * 2) + 1.
     Each view has only half the amount of lines as noted in the
     height property, pads specifying the "fields" property
     must be prepared for this. This mode requires multiple
     GstVideoMeta metadata to describe the fields.

 - **chroma-site**, `G_TYPE_STRING`: Default UNKNOWN. The chroma siting of the
   video frames.

   - *"jpeg"*: `GST_VIDEO_CHROMA_SITE_JPEG`
   - *"mpeg2"*: `GST_VIDEO_CHROMA_SITE_MPEG2`
   - *"dv"*: `GST_VIDEO_CHROMA_SITE_DV`

 - **colorimetry**, `G_TYPE_STRING`: Default UNKNOWN. The colorimetry of the
   video frames. Predefined colorimetry is given with the following values:

   - *"bt601"*
   - *"bt709"*
   - *"smpte240m"*

 - **pixel-aspect-ratio**, `GST_TYPE_FRACTION`: Default 1/1. The pixel aspect
   ration of the video

 - **format**, `G_TYPE_STRING`: Mandatory. The format of the video. See the
   Formats section for a list of valid format strings.

## Metadata

 - `GstVideoMeta` contains the description of one video field or frame. It
   has stride support and support for having multiple memory regions per frame.
   Multiple GstVideoMeta can be added to a buffer and can be identified with a
   unique id. This id can be used to select fields in interlaced formats or
   views in multiview formats.

 - `GstVideoCropMeta` contains the cropping region of the video.

## Formats

- **"I420"** planar 4:2:0 YUV

```
        Component 0: Y
          depth:           8
          pstride:         1
          default offset:  0
          default rstride: RU4 (width)
          default size:    rstride (component0) * RU2 (height)

        Component 1: U
          depth:           8
          pstride:         1
          default offset:  size (component0)
          default rstride: RU4 (RU2 (width) / 2)
          default size:    rstride (component1) * RU2 (height) / 2

        Component 2: V
          depth            8
          pstride:         1
          default offset:  offset (component1) + size (component1)
          default rstride: RU4 (RU2 (width) / 2)
          default size:    rstride (component2) * RU2 (height) / 2

        Image
          default size: size (component0) +
                        size (component1) +
                        size (component2)
```

- **"YV12"** planar 4:2:0 YUV

        Same as I420 but with U and V planes swapped

```
        Component 0: Y
          depth:           8
          pstride:         1
          default offset:  0
          default rstride: RU4 (width)
          default size:    rstride (component0) * RU2 (height)

        Component 1: U
          depth            8
          pstride:         1
          default offset:  offset (component2) + size (component2)
          default rstride: RU4 (RU2 (width) / 2)
          default size:    rstride (component1) * RU2 (height) / 2

        Component 2: V
          depth:           8
          pstride:         1
          default offset:  size (component0)
          default rstride: RU4 (RU2 (width) / 2)
          default size:    rstride (component2) * RU2 (height) / 2

        Image
          default size: size (component0) +
                        size (component1) +
                        size (component2)
```

- **"YUY2"** packed 4:2:2 YUV

```
       +--+--+--+--+ +--+--+--+--+
       |Y0|U0|Y1|V0| |Y2|U2|Y3|V2| ...
       +--+--+--+--+ +--+--+--+--+

        Component 0: Y
          depth:           8
          pstride:         2
          offset:          0

        Component 1: U
          depth:           8
          offset:          1
          pstride:         4

        Component 2: V
          depth            8
          offset:          3
          pstride:         4

        Image
          default rstride: RU4 (width * 2)
          default size:    rstride (image) * height
```

- **"YVYU"** packed 4:2:2 YUV

      Same as "YUY2" but with U and V planes swapped

```
       +--+--+--+--+ +--+--+--+--+
       |Y0|V0|Y1|U0| |Y2|V2|Y3|U2| ...
       +--+--+--+--+ +--+--+--+--+

        Component 0: Y
          depth:           8
          pstride:         2
          offset:          0

        Component 1: U
          depth:           8
          pstride:         4
          offset:          3

        Component 2: V
          depth            8
          pstride:         4
          offset:          1

        Image
          default rstride: RU4 (width * 2)
          default size:    rstride (image) * height
```

- **"UYVY"** packed 4:2:2 YUV

```
       +--+--+--+--+ +--+--+--+--+
       |U0|Y0|V0|Y1| |U2|Y2|V2|Y3| ...
       +--+--+--+--+ +--+--+--+--+

        Component 0: Y
          depth:           8
          pstride:         2
          offset:          1

        Component 1: U
          depth:           8
          pstride:         4
          offset:          0

        Component 2: V
          depth            8
          pstride:         4
          offset:          2

        Image
          default rstride: RU4 (width * 2)
          default size:    rstride (image) * height
```

- **"AYUV"** packed 4:4:4 YUV with alpha channel

```
       +--+--+--+--+ +--+--+--+--+
       |A0|Y0|U0|V0| |A1|Y1|U1|V1| ...
       +--+--+--+--+ +--+--+--+--+

        Component 0: Y
          depth:           8
          pstride:         4
          offset:          1

        Component 1: U
          depth:           8
          pstride:         4
          offset:          2

        Component 2: V
          depth            8
          pstride:         4
          offset:          3

        Component 3: A
          depth            8
          pstride:         4
          offset:          0

        Image
          default rstride: width * 4
          default size:    rstride (image) * height
```

- **"RGBx"** sparse rgb packed into 32 bit, space last

```
       +--+--+--+--+ +--+--+--+--+
       |R0|G0|B0|X | |R1|G1|B1|X | ...
       +--+--+--+--+ +--+--+--+--+

        Component 0: R
          depth:           8
          pstride:         4
          offset:          0

        Component 1: G
          depth:           8
          pstride:         4
          offset:          1

        Component 2: B
          depth            8
          pstride:         4
          offset:          2

        Image
          default rstride: width * 4
          default size:    rstride (image) * height
```

- **"BGRx"** sparse reverse rgb packed into 32 bit, space last

```
       +--+--+--+--+ +--+--+--+--+
       |B0|G0|R0|X | |B1|G1|R1|X | ...
       +--+--+--+--+ +--+--+--+--+

        Component 0: R
          depth:           8
          pstride:         4
          offset:          2

        Component 1: G
          depth:           8
          pstride:         4
          offset:          1

        Component 2: B
          depth            8
          pstride:         4
          offset:          0

        Image
          default rstride: width * 4
          default size:    rstride (image) * height
```

- **"xRGB"** sparse rgb packed into 32 bit, space first

```
       +--+--+--+--+ +--+--+--+--+
       |X |R0|G0|B0| |X |R1|G1|B1| ...
       +--+--+--+--+ +--+--+--+--+

        Component 0: R
          depth:           8
          pstride:         4
          offset:          1

        Component 1: G
          depth:           8
          pstride:         4
          offset:          2

        Component 2: B
          depth            8
          pstride:         4
          offset:          3

        Image
          default rstride: width * 4
          default size:    rstride (image) * height
```

- **"xBGR"** sparse reverse rgb packed into 32 bit, space first

```
       +--+--+--+--+ +--+--+--+--+
       |X |B0|G0|R0| |X |B1|G1|R1| ...
       +--+--+--+--+ +--+--+--+--+

        Component 0: R
          depth:           8
          pstride:         4
          offset:          3

        Component 1: G
          depth:           8
          pstride:         4
          offset:          2

        Component 2: B
          depth            8
          pstride:         4
          offset:          1

        Image
          default rstride: width * 4
          default size:    rstride (image) * height
```

- **"RGBA"** rgb with alpha channel last

```
       +--+--+--+--+ +--+--+--+--+
       |R0|G0|B0|A0| |R1|G1|B1|A1| ...
       +--+--+--+--+ +--+--+--+--+

        Component 0: R
          depth:           8
          pstride:         4
          offset:          0

        Component 1: G
          depth:           8
          pstride:         4
          offset:          1

        Component 2: B
          depth            8
          pstride:         4
          offset:          2

        Component 3: A
          depth            8
          pstride:         4
          offset:          3

        Image
          default rstride: width * 4
          default size:    rstride (image) * height
```

- **"BGRA"** reverse rgb with alpha channel last

```
       +--+--+--+--+ +--+--+--+--+
       |B0|G0|R0|A0| |B1|G1|R1|A1| ...
       +--+--+--+--+ +--+--+--+--+

        Component 0: R
          depth:           8
          pstride:         4
          offset:          2

        Component 1: G
          depth:           8
          pstride:         4
          offset:          1

        Component 2: B
          depth            8
          pstride:         4
          offset:          0

        Component 3: A
          depth            8
          pstride:         4
          offset:          3

        Image
          default rstride: width * 4
          default size:    rstride (image) * height
```

- **"ARGB"** rgb with alpha channel first

```
       +--+--+--+--+ +--+--+--+--+
       |A0|R0|G0|B0| |A1|R1|G1|B1| ...
       +--+--+--+--+ +--+--+--+--+

        Component 0: R
          depth:           8
          pstride:         4
          offset:          1

        Component 1: G
          depth:           8
          pstride:         4
          offset:          2

        Component 2: B
          depth            8
          pstride:         4
          offset:          3

        Component 3: A
          depth            8
          pstride:         4
          offset:          0

        Image
          default rstride: width * 4
          default size:    rstride (image) * height
```

- **"ABGR"** reverse rgb with alpha channel first

```
       +--+--+--+--+ +--+--+--+--+
       |A0|R0|G0|B0| |A1|R1|G1|B1| ...
       +--+--+--+--+ +--+--+--+--+

        Component 0: R
          depth:           8
          pstride:         4
          offset:          1

        Component 1: G
          depth:           8
          pstride:         4
          offset:          2

        Component 2: B
          depth            8
          pstride:         4
          offset:          3

        Component 3: A
          depth            8
          pstride:         4
          offset:          0

        Image
          default rstride: width * 4
          default size:    rstride (image) * height
```

- **"RGB"** rgb

```
       +--+--+--+ +--+--+--+
       |R0|G0|B0| |R1|G1|B1| ...
       +--+--+--+ +--+--+--+

        Component 0: R
          depth:           8
          pstride:         3
          offset:          0

        Component 1: G
          depth:           8
          pstride:         3
          offset:          1

        Component 2: B
          depth            8
          pstride:         3
          offset:          2

        Image
          default rstride: RU4 (width * 3)
          default size:    rstride (image) * height
```

- **"BGR"** reverse rgb

```
       +--+--+--+ +--+--+--+
       |B0|G0|R0| |B1|G1|R1| ...
       +--+--+--+ +--+--+--+

        Component 0: R
          depth:           8
          pstride:         3
          offset:          2

        Component 1: G
          depth:           8
          pstride:         3
          offset:          1

        Component 2: B
          depth            8
          pstride:         3
          offset:          0

        Image
          default rstride: RU4 (width * 3)
          default size:    rstride (image) * height
```

- **"Y41B"** planar 4:1:1 YUV

```
        Component 0: Y
          depth:           8
          pstride:         1
          default offset:  0
          default rstride: RU4 (width)
          default size:    rstride (component0) * height

        Component 1: U
          depth            8
          pstride:         1
          default offset:  size (component0)
          default rstride: RU16 (width) / 4
          default size:    rstride (component1) * height

        Component 2: V
          depth:           8
          pstride:         1
          default offset:  offset (component1) + size (component1)
          default rstride: RU16 (width) / 4
          default size:    rstride (component2) * height

        Image
          default size: size (component0) +
                        size (component1) +
                        size (component2)
```

- **"Y42B"** planar 4:2:2 YUV

```
        Component 0: Y
          depth:           8
          pstride:         1
          default offset:  0
          default rstride: RU4 (width)
          default size:    rstride (component0) * height

        Component 1: U
          depth            8
          pstride:         1
          default offset:  size (component0)
          default rstride: RU8 (width) / 2
          default size:    rstride (component1) * height

        Component 2: V
          depth:           8
          pstride:         1
          default offset:  offset (component1) + size (component1)
          default rstride: RU8 (width) / 2
          default size:    rstride (component2) * height

        Image
          default size: size (component0) +
                        size (component1) +
                        size (component2)
```

- **"Y444"** planar 4:4:4 YUV

```
        Component 0: Y
          depth:           8
          pstride:         1
          default offset:  0
          default rstride: RU4 (width)
          default size:    rstride (component0) * height

        Component 1: U
          depth            8
          pstride:         1
          default offset:  size (component0)
          default rstride: RU4 (width)
          default size:    rstride (component1) * height

        Component 2: V
          depth:           8
          pstride:         1
          default offset:  offset (component1) + size (component1)
          default rstride: RU4 (width)
          default size:    rstride (component2) * height

        Image
          default size: size (component0) +
                        size (component1) +
                        size (component2)

- **"v210"** packed 4:2:2 10-bit YUV, complex format

```
        Component 0: Y
          depth:           10

        Component 1: U
          depth            10

        Component 2: V
          depth:           10

        Image
          default rstride: RU48 (width) * 128
          default size:    rstride (image) * height
```

- **"v216"** packed 4:2:2 16-bit YUV, Y0-U0-Y1-V1 order

```
       +--+--+--+--+ +--+--+--+--+
       |U0|Y0|V0|Y1| |U1|Y2|V1|Y3| ...
       +--+--+--+--+ +--+--+--+--+

        Component 0: Y
          depth:           16 LE
          pstride:         4
          offset:          2

        Component 1: U
          depth            16 LE
          pstride:         8
          offset:          0

        Component 2: V
          depth:           16 LE
          pstride:         8
          offset:          4

        Image
          default rstride: RU8 (width * 2)
          default size:    rstride (image) * height
```

- **"NV12"** planar 4:2:0 YUV with interleaved UV plane

```
        Component 0: Y
          depth:           8
          pstride:         1
          default offset:  0
          default rstride: RU4 (width)
          default size:    rstride (component0) * RU2 (height)

        Component 1: U
          depth            8
          pstride:         2
          default offset:  size (component0)
          default rstride: RU4 (width)

        Component 2: V
          depth:           8
          pstride:         2
          default offset:  offset (component1) + 1
          default rstride: RU4 (width)

        Image
          default size: RU4 (width) * RU2 (height) * 3 / 2
```

- **"NV21"** planar 4:2:0 YUV with interleaved VU plane

```
        Component 0: Y
          depth:           8
          pstride:         1
          default offset:  0
          default rstride: RU4 (width)
          default size:    rstride (component0) * RU2 (height)

        Component 1: U
          depth            8
          pstride:         2
          default offset:  offset (component1) + 1
          default rstride: RU4 (width)

        Component 2: V
          depth:           8
          pstride:         2
          default offset:  size (component0)
          default rstride: RU4 (width)

        Image
          default size: RU4 (width) * RU2 (height) * 3 / 2
```

- **"GRAY8"** 8-bit grayscale "Y800" same as "GRAY8"

```
        Component 0: Y
          depth:           8
          offset:          0
          pstride:         1
          default rstride: RU4 (width)
          default size:    rstride (component0) * height

        Image
          default size:    size (component0)
```

- **"GRAY16\_BE"** 16-bit grayscale, most significant byte first

```
        Component 0: Y
          depth:           16
          offset:          0
          pstride:         2
          default rstride: RU4 (width * 2)
          default size:    rstride (component0) * height

        Image
          default size:    size (component0)
```

- **"GRAY16\_LE"** 16-bit grayscale, least significant byte first
- **"Y16"** same as "GRAY16\_LE"

```
        Component 0: Y
          depth:           16 LE
          offset:          0
          pstride:         2
          default rstride: RU4 (width * 2)
          default size:    rstride (component0) * height

        Image
          default size:    size (component0)
```

- **"v308"** packed 4:4:4 YUV

```
       +--+--+--+ +--+--+--+
       |Y0|U0|V0| |Y1|U1|V1| ...
       +--+--+--+ +--+--+--+

        Component 0: Y
          depth:           8
          pstride:         3
          offset:          0

        Component 1: U
          depth            8
          pstride:         3
          offset:          1

        Component 2: V
          depth:           8
          pstride:         3
          offset:          2

        Image
          default rstride: RU4 (width * 3)
          default size:    rstride (image) * height
```

- **"IYU2"** packed 4:4:4 YUV, U-Y-V order

```
       +--+--+--+ +--+--+--+
       |U0|Y0|V0| |U1|Y1|V1| ...
       +--+--+--+ +--+--+--+

        Component 0: Y
          depth:           8
          pstride:         3
          offset:          1

        Component 1: U
          depth            8
          pstride:         3
          offset:          0

        Component 2: V
          depth:           8
          pstride:         3
          offset:          2

        Image
          default rstride: RU4 (width * 3)
          default size:    rstride (image) * height
```

- **"RGB16"** rgb 5-6-5 bits per component

```
       +--+--+--+ +--+--+--+
       |R0|G0|B0| |R1|G1|B1| ...
       +--+--+--+ +--+--+--+

        Component 0: R
          depth:           5
          pstride:         2

        Component 1: G
          depth            6
          pstride:         2

        Component 2: B
          depth:           5
          pstride:         2

        Image
          default rstride: RU4 (width * 2)
          default size:    rstride (image) * height
```

- **"BGR16"** reverse rgb 5-6-5 bits per component

```
       +--+--+--+ +--+--+--+
       |B0|G0|R0| |B1|G1|R1| ...
       +--+--+--+ +--+--+--+

        Component 0: R
          depth:           5
          pstride:         2

        Component 1: G
          depth            6
          pstride:         2

        Component 2: B
          depth:           5
          pstride:         2

        Image
          default rstride: RU4 (width * 2)
          default size:    rstride (image) * height
```

- **"RGB15"** rgb 5-5-5 bits per component

```
       +--+--+--+ +--+--+--+
       |R0|G0|B0| |R1|G1|B1| ...
       +--+--+--+ +--+--+--+

        Component 0: R
          depth:           5
          pstride:         2

        Component 1: G
          depth            5
          pstride:         2

        Component 2: B
          depth:           5
          pstride:         2

        Image
          default rstride: RU4 (width * 2)
          default size:    rstride (image) * height
```

- **"BGR15"** reverse rgb 5-5-5 bits per component

```
       +--+--+--+ +--+--+--+
       |B0|G0|R0| |B1|G1|R1| ...
       +--+--+--+ +--+--+--+

        Component 0: R
          depth:           5
          pstride:         2

        Component 1: G
          depth            5
          pstride:         2

        Component 2: B
          depth:           5
          pstride:         2

        Image
          default rstride: RU4 (width * 2)
          default size:    rstride (image) * height
```

- **"UYVP"** packed 10-bit 4:2:2 YUV (U0-Y0-V0-Y1 U2-Y2-V2-Y3 U4 ...)

```
        Component 0: Y
          depth:           10

        Component 1: U
          depth            10

        Component 2: V
          depth:           10

        Image
          default rstride: RU4 (width * 2 * 5)
          default size:    rstride (image) * height
```

- **"A420"** planar 4:4:2:0 AYUV

```
        Component 0: Y
          depth:           8
          pstride:         1
          default offset:  0
          default rstride: RU4 (width)
          default size:    rstride (component0) * RU2 (height)

        Component 1: U
          depth            8
          pstride:         1
          default offset:  size (component0)
          default rstride: RU4 (RU2 (width) / 2)
          default size:    rstride (component1) * (RU2 (height) / 2)

        Component 2: V
          depth:           8
          pstride:         1
          default offset:  size (component0) + size (component1)
          default rstride: RU4 (RU2 (width) / 2)
          default size:    rstride (component2) * (RU2 (height) / 2)

        Component 3: A
          depth:           8
          pstride:         1
          default offset:  size (component0) + size (component1) +
                           size (component2)
          default rstride: RU4 (width)
          default size:    rstride (component3) * RU2 (height)

        Image
          default size:    size (component0) +
                           size (component1) +
                           size (component2) +
                           size (component3)
```

- **"RGB8P"** 8-bit paletted RGB

```
        Component 0: INDEX
          depth:           8
          pstride:         1
          default offset:  0
          default rstride: RU4 (width)
          default size:    rstride (component0) * height

        Component 1: PALETTE
          depth            32
          pstride:         4
          default offset:  size (component0)
          rstride:         4
          size:            256 * 4

        Image
          default size:    size (component0) + size (component1)
```

- **"YUV9"** planar 4:1:0 YUV

```
        Component 0: Y
          depth:           8
          pstride:         1
          default offset:  0
          default rstride: RU4 (width)
          default size:    rstride (component0) * height

        Component 1: U
          depth            8
          pstride:         1
          default offset:  size (component0)
          default rstride: RU4 (RU4 (width) / 4)
          default size:    rstride (component1) * (RU4 (height) / 4)

        Component 2: V
          depth:           8
          pstride:         1
          default offset:  offset (component1) + size (component1)
          default rstride: RU4 (RU4 (width) / 4)
          default size:    rstride (component2) * (RU4 (height) / 4)

        Image
          default size: size (component0) +
                        size (component1) +
                        size (component2)
```

- **"YVU9"** planar 4:1:0 YUV (like YUV9 but UV planes swapped)

```
        Component 0: Y
          depth:           8
          pstride:         1
          default offset:  0
          default rstride: RU4 (width)
          default size:    rstride (component0) * height

        Component 1: U
          depth            8
          pstride:         1
          default offset:  offset (component2) + size (component2)
          default rstride: RU4 (RU4 (width) / 4)
          default size:    rstride (component1) * (RU4 (height) / 4)

        Component 2: V
          depth:           8
          pstride:         1
          default offset:  size (component0)
          default rstride: RU4 (RU4 (width) / 4)
          default size:    rstride (component2) * (RU4 (height) / 4)

        Image
          default size: size (component0) +
                        size (component1) +
                        size (component2)
```

- **"IYU1"** packed 4:1:1 YUV (Cb-Y0-Y1-Cr-Y2-Y3 ...)

```
       +--+--+--+ +--+--+--+
       |B0|G0|R0| |B1|G1|R1| ...
       +--+--+--+ +--+--+--+

        Component 0: Y
          depth:           8
          offset:          1
          pstride:         2

        Component 1: U
          depth            5
          offset:          0
          pstride:         2

        Component 2: V
          depth:           5
          offset:          4
          pstride:         2

        Image
          default rstride: RU4 (RU4 (width) + RU4 (width) / 2)
          default size:    rstride (image) * height
```

- **"ARGB64"** rgb with alpha channel first, 16 bits per channel

```
       +--+--+--+--+ +--+--+--+--+
       |A0|R0|G0|B0| |A1|R1|G1|B1| ...
       +--+--+--+--+ +--+--+--+--+

        Component 0: R
          depth:           16 LE
          pstride:         8
          offset:          2

        Component 1: G
          depth            16 LE
          pstride:         8
          offset:          4

        Component 2: B
          depth:           16 LE
          pstride:         8
          offset:          6

        Component 3: A
          depth:           16 LE
          pstride:         8
          offset:          0

        Image
          default rstride: width * 8
          default size:    rstride (image) * height
```

- **"AYUV64"** packed 4:4:4 YUV with alpha channel, 16 bits per channel (A0-Y0-U0-V0 ...)

```
       +--+--+--+--+ +--+--+--+--+
       |A0|Y0|U0|V0| |A1|Y1|U1|V1| ...
       +--+--+--+--+ +--+--+--+--+

        Component 0: Y
          depth:           16 LE
          pstride:         8
          offset:          2

        Component 1: U
          depth            16 LE
          pstride:         8
          offset:          4

        Component 2: V
          depth:           16 LE
          pstride:         8
          offset:          6

        Component 3: A
          depth:           16 LE
          pstride:         8
          offset:          0

        Image
          default rstride: width * 8
          default size:    rstride (image) * height
```

- **"r210"** packed 4:4:4 RGB, 10 bits per channel

```
       +--+--+--+ +--+--+--+
       |R0|G0|B0| |R1|G1|B1| ...
       +--+--+--+ +--+--+--+

        Component 0: R
          depth:           10
          pstride:         4

        Component 1: G
          depth            10
          pstride:         4

        Component 2: B
          depth:           10
          pstride:         4

        Image
          default rstride: width * 4
          default size:    rstride (image) * height
```

- **"I420\_10LE"** planar 4:2:0 YUV, 10 bits per channel LE

```
        Component 0: Y
          depth:           10 LE
          pstride:         2
          default offset:  0
          default rstride: RU4 (width * 2)
          default size:    rstride (component0) * RU2 (height)

        Component 1: U
          depth:           10 LE
          pstride:         2
          default offset:  size (component0)
          default rstride: RU4 (width)
          default size:    rstride (component1) * RU2 (height) / 2

        Component 2: V
          depth            10 LE
          pstride:         2
          default offset:  offset (component1) + size (component1)
          default rstride: RU4 (width)
          default size:    rstride (component2) * RU2 (height) / 2

        Image
          default size: size (component0) +
                        size (component1) +
                        size (component2)
```

- **"I420\_10BE"** planar 4:2:0 YUV, 10 bits per channel BE

```
        Component 0: Y
          depth:           10 BE
          pstride:         2
          default offset:  0
          default rstride: RU4 (width * 2)
          default size:    rstride (component0) * RU2 (height)

        Component 1: U
          depth:           10 BE
          pstride:         2
          default offset:  size (component0)
          default rstride: RU4 (width)
          default size:    rstride (component1) * RU2 (height) / 2

        Component 2: V
          depth            10 BE
          pstride:         2
          default offset:  offset (component1) + size (component1)
          default rstride: RU4 (width)
          default size:    rstride (component2) * RU2 (height) / 2

        Image
          default size: size (component0) +
                        size (component1) +
                        size (component2)
```

- **"I422\_10LE"** planar 4:2:2 YUV, 10 bits per channel LE

```
        Component 0: Y
          depth:           10 LE
          pstride:         2
          default offset:  0
          default rstride: RU4 (width * 2)
          default size:    rstride (component0) * RU2 (height)

        Component 1: U
          depth:           10 LE
          pstride:         2
          default offset:  size (component0)
          default rstride: RU4 (width)
          default size:    rstride (component1) * RU2 (height)

        Component 2: V
          depth            10 LE
          pstride:         2
          default offset:  offset (component1) + size (component1)
          default rstride: RU4 (width)
          default size:    rstride (component2) * RU2 (height)

        Image
          default size: size (component0) +
                        size (component1) +
                        size (component2)
```

- **"I422\_10BE"** planar 4:2:2 YUV, 10 bits per channel BE

```
        Component 0: Y
          depth:           10 BE
          pstride:         2
          default offset:  0
          default rstride: RU4 (width * 2)
          default size:    rstride (component0) * RU2 (height)

        Component 1: U
          depth:           10 BE
          pstride:         2
          default offset:  size (component0)
          default rstride: RU4 (width)
          default size:    rstride (component1) * RU2 (height)

        Component 2: V
          depth            10 BE
          pstride:         2
          default offset:  offset (component1) + size (component1)
          default rstride: RU4 (width)
          default size:    rstride (component2) * RU2 (height)

        Image
          default size: size (component0) +
                        size (component1) +
                        size (component2)
```

- **"Y444\_10BE"** planar 4:4:4 YUV, 10 bits per channel
- **"Y444\_10LE"** planar 4:4:4 YUV, 10 bits per channel

- **"GBR"** planar 4:4:4 RGB, 8 bits per channel
- **"GBR\_10BE"** planar 4:4:4 RGB, 10 bits per channel
- **"GBR\_10LE"** planar 4:4:4 RGB, 10 bits per channel

- **"NV16"** planar 4:2:2 YUV with interleaved UV plane
- **"NV61"** planar 4:2:2 YUV with interleaved VU plane
- **"NV24"** planar 4:4:4 YUV with interleaved UV plane

- **"NV12\_64Z32"** planar 4:2:0 YUV with interleaved UV plane in 64x32 tiles zigzag

```
        Component 0: Y
          depth:           8
          pstride:         1
          default offset:  0
          default rstride: RU128 (width)
          default size:    rstride (component0) * RU32 (height)

        Component 1: U
          depth            8
          pstride:         2
          default offset:  size (component0)
          default rstride: (y_tiles << 16) | x_tiles
          default x_tiles: RU128 (width) >> tile_width
          default y_tiles: RU32 (height) >> tile_height

        Component 2: V
          depth:           8
          pstride:         2
          default offset:  offset (component1) + 1
          default rstride: (y_tiles << 16) | x_tiles
          default x_tiles: RU128 (width) >> tile_width
          default y_tiles: RU64 (height) >> (tile_height + 1)

        Image
          default size: RU128 (width) * (RU32 (height) + RU64 (height) / 2)
          tile mode:    ZFLIPZ_2X2
          tile width:   6
          tile height:  5
```
