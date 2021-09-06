# Contributing to libfprint

## GLib

Although the library uses GLib internally, libfprint is designed to provide
a completely neutral interface to its application users. So, the public
APIs should never return GLib data types.

## License clarification

Although this library's license could allow for shims that hook up into
proprietary blobs to add driver support for some unsupported devices, the
intent of the original authors, and of current maintainers of the library,
was for this license to allow _integration into_ proprietary stacks, not
_integration of_ proprietary code in the library.

As such, no code to integrate proprietary drivers will be accepted in libfprint
upstream. Proprietary drivers would make it impossible to debug problems in
libfprint, as we wouldn't know what the proprietary driver does behind the
library's back. The closed source nature of drivers is usually used to hide
parts of the hardware setup, such as encryption keys, or protocols, in order
to protect the hardware's integrity. Unfortunately, this is only [security through
obscurity](https://en.wikipedia.org/wiki/Security_through_obscurity).

We however encourage potential contributors to take advantage of libfprint's
source availability to create such shims to make it easier to reverse-engineer
proprietary drivers in order to create new free software drivers, to the extent
permitted by local laws.

## Two-faced-ness

Like any decent library, this one is designed to provide a stable and
documented API to its users: applications. Clear distinction is made between
data available internally in the library, and data/functions available to
the applications.

This library is confused a little by the fact that there is another 'interface'
at hand: the internal interface provided to drivers. So, we effectively end
up with 2 APIs:

 1. The [external-facing API for applications](libfprint/fprint.h)
 2. The [internal API for fingerprint drivers](libfprint/drivers_api.h)

Non-static functions which are intended for internal use only are prepended
with the "fpi_" prefix.

## Documentation

All additions of public API functions must be accompanied with gtk-doc
comments.

All changes which potentially change the behaviour of the public API must
be reflected by updating the appropriate gtk-doc comments.


## Contributing

Patches should be sent as merge requests to the gitlab page:
https://gitlab.freedesktop.org/libfprint/libfprint/merge_requests

Drivers are not usually written by libfprint developers, but when they
are, we require:
- 3 stand-alone devices. Not in a laptop or another embedded device, as
  space is scarce, unless the device has special integration with that
  hardware.
- specifications of the protocol.

If you are an end-user, you can file a feature request with the "Driver Request"
tag on [libfprint's issue page](https://gitlab.freedesktop.org/libfprint/libfprint/issues?scope=all&utf8=%E2%9C%93&state=opened&label_name[]=Driver%20Request),
or subscribe to an existing feature request there.

If you are an enterprising hacker, please file a new merge request with
the driver patches integrated.
