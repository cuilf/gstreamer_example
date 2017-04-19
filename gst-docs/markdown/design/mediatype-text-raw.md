# Raw Text Media Types

**text/x-raw**

 - **format**, `G_TYPE_STRING`: Mandatory. The format of the text. See the
   Formats section for a list of valid format strings.

## Metadata

There are no common metas for this raw format yet.

## Formats

 - "utf8": plain timed utf8 text (formerly text/plain)
   Parsed timed text in utf8 format.

 - "pango-markup": plain timed utf8 text with pango markup
   (formerly text/x-pango-markup). Same as "utf8", but text embedded in an
   XML-style markup language for size, colour, emphasis, etc.
   See [Pango Markup Format][pango-markup]

[pango-markup]: http://developer.gnome.org/pango/stable/PangoMarkupFormat.html
