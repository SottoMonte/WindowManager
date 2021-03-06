/*
 * IceWM
 *
 * Copyright (C) 1997-2001 Marko Macek
 */
#include "config.h"
#include "yfull.h"
#include "ypixbuf.h"
#include "ypaint.h"
#include "yxapp.h"
#include "sysdep.h"
#include "prefs.h"
#include "yprefs.h"
#include "wmprog.h" // !!! remove this

#include "yimage.h"
#include <gdk-pixbuf-xlib/gdk-pixbuf-xlib.h>

#ifdef CONFIG_XPM
#include <X11/xpm.h>
#endif

#include "intl.h"

#if 0
#if 0
YPixmap::YPixmap(YPixmap const & pixmap): refcounted(),
    fPixmap(pixmap.fPixmap), fMask(pixmap.fMask),
    fWidth(pixmap.fWidth), fHeight(pixmap.fHeight),
    fOwned(false) {
    }
#endif

#if 0
#ifdef CONFIG_ANTIALIASING
YPixmap::YPixmap(YPixbuf & pixbuf):
    fWidth(pixbuf.width()), fHeight(pixbuf.height()),
    fPixmap(createPixmap(pixbuf.width(), pixbuf.height())),
    fMask(pixbuf.alpha() ? createMask(pixbuf.width(), pixbuf.height()) : None),
    fOwned(true) 
{
    Graphics(fPixmap, pixbuf.width(), pixbuf.height()).copyPixbuf(pixbuf, 0, 0, fWidth, fHeight, 0, 0, false);
    Graphics(fMask, pixbuf.width(), pixbuf.height()).copyAlphaMask(pixbuf, 0, 0, fWidth, fHeight, 0, 0);
}
#endif
#endif

YPixmap::YPixmap(upath filename): fOwned(true) {
    cstring cs(filename.path());
#if defined(CONFIG_IMLIB)
    ImlibImage *im(Imlib_load_image(hImlib, (char *)REDIR_ROOT(cs.c_str())));

    if (im) {
        fWidth = im->rgb_width;
        fHeight = im->rgb_height;
        Imlib_render(hImlib, im, fWidth, fHeight);
        fPixmap = (Pixmap)Imlib_move_image(hImlib, im);
        fMask = (Pixmap)Imlib_move_mask(hImlib, im);
        Imlib_destroy_image(hImlib, im);
    } else {
        warn(_("Loading of image \"%s\" failed"), cs.c_str());
        fPixmap = fMask = None;
        fWidth = fHeight = 16;
    }
    MSG(("%s %d %d", cs.c_str(), fWidth, fHeight));
#elif defined(CONFIG_XPM)
    XpmAttributes xpmAttributes;
    memset(&xpmAttributes, 0, sizeof(xpmAttributes));
    xpmAttributes.colormap  = xapp->colormap();
    xpmAttributes.closeness = 65535;
    xpmAttributes.valuemask = XpmSize|XpmReturnPixels|XpmColormap|XpmCloseness;

    int const rc(XpmReadFileToPixmap(xapp->display(), desktop->handle(),
                                     (char *)REDIR_ROOT(cstring(filename.path()).c_str()), // !!!
                                     &fPixmap, &fMask, &xpmAttributes));
    if (rc == XpmSuccess) {
        fWidth = xpmAttributes.width;
        fHeight = xpmAttributes.height;
        XpmFreeAttributes(&xpmAttributes);
    } else {
        warn(_("Loading of pixmap \"%s\" failed: %s"),
               cstring(filename.path()).c_str(), XpmGetErrorString(rc));

        fWidth = fHeight = 16; /// should be 0, fix
        fPixmap = fMask = None;
    }
#else
    fWidth = fHeight = 16; /// should be 0, fix
    fPixmap = fMask = None;
#endif
}
#endif

#if 0
#ifdef CONFIG_IMLIB
/* Load pixmap at specified size */
YPixmap::YPixmap(const char *filename, int w, int h) {
    fOwned = true;
    fWidth = w;
    fHeight = h;

    ImlibImage *im = Imlib_load_image(hImlib, (char *)REDIR_ROOT(filename));
    if(im) {
        Imlib_render(hImlib, im, fWidth, fHeight);
        fPixmap = (Pixmap) Imlib_move_image(hImlib, im);
        fMask = (Pixmap) Imlib_move_mask(hImlib, im);
        Imlib_destroy_image(hImlib, im);
    } else {
        warn(_("Loading of image \"%s\" failed"), filename);
        fPixmap = fMask = None;
    }
    MSG(("%s %d %d", filename, fWidth, fHeight));
}
#endif
#endif

#if 0
YPixmap::YPixmap(bool owned, Pixmap pixmap, Pixmap mask, int w, int h) {
    fOwned = owned;
    fWidth = w;
    fHeight = h;
    fPixmap = pixmap;
    fMask = mask;
}
#endif

#ifdef CONFIG_IMLIB

#if 0
void YPixmap::scaleImage(Pixmap pixmap, Pixmap mask,
                         int x, int y, int w, int h, int nw, int nh)
{
//    abort();

#if 0
    if (pixmap != None) {
        ImlibImage *im =
            Imlib_create_image_from_drawable (hImlib, pixmap, 0, x, y, w, h);

        if (im == 0) {
            warn (_("Imlib: Acquisition of X pixmap failed"));
            return;
        }

        Imlib_render(hImlib, im, nw, nh);
        fPixmap = Imlib_move_image(hImlib, im);
        Imlib_destroy_image(hImlib, im);

        if (fPixmap == 0) {
            warn (_("Imlib: Imlib image to X pixmap mapping failed"));
            return;
        }
    }
    if (mask != None) {
        ImlibImage *im =
            Imlib_create_image_from_drawable (hImlib, mask, 0, x, y, w, h);

        if (im == 0) {
            warn (_("Imlib: Acquisition of X pixmap failed"));
            return;
        }
//
// Initialization of a bilevel pixmap
//
        ImlibImage *sc =
            Imlib_clone_scaled_image (hImlib, im, nw, nh);
        Imlib_destroy_image(hImlib, im);

        fMask = createMask(nw, nh);
        Graphics g(fMask, nw, nh);

        g.setColorPixel(1);
        g.fillRect(0, 0, nw, nh);

        g.setColorPixel(0);
//
// nested rendering loop inspired by gdk-pixbuf
//
        unsigned char *px = sc->rgb_data;
        for (int y = 0; y < nh; ++y)
            for (int xa = 0, xe; xa < nw; xa = xe) {
                while (xa < nw && *px < 128) ++xa, px+= 3;
                xe = xa;
                while (xe < nw && *px >= 128) ++xe, px+= 3;
                g.drawLine(xa, y, xe - 1, y);
            }

        Imlib_destroy_image(hImlib, sc);
    }
#endif
}
#endif

#if 0
YPixmap::YPixmap(const ref<YPixmap> &pixmap, int wScaled, int hScaled) {
    fOwned = true;
    fWidth = wScaled;
    fHeight = hScaled;
    fPixmap = fMask = None;

    scaleImage(pixmap->pixmap(), pixmap->mask(),
               0, 0, pixmap->width(), pixmap->height(),
               fWidth,
               fHeight);
}

YPixmap::YPixmap(Pixmap pixmap, Pixmap mask, int w, int h,
                 int wScaled, int hScaled)
{
    fOwned = true;
    fWidth = wScaled;
    fHeight = hScaled;
    fPixmap = fMask = None;

    scaleImage(pixmap, mask,
               0, 0, w, h,
               fWidth,
               fHeight);
}
#endif
#endif

#if 0
YPixmap::YPixmap(int w, int h, bool mask) {
    fOwned = true;
    fWidth = w;
    fHeight = h;

    fPixmap = createPixmap(fWidth, fHeight);
    fMask = mask ? createPixmap(fWidth, fHeight) : None;
}

YPixmap::~YPixmap() {
    if (fOwned) {
        if (fPixmap != None) {
            if (xapp != 0)
                XFreePixmap(xapp->display(), fPixmap);
            fPixmap = 0;
        }
        if (fMask != None) {
            if (xapp != 0)
                XFreePixmap(xapp->display(), fMask);
            fMask = 0;
        }
    }
}

void YPixmap::replicate(bool horiz, bool copyMask) {
    if (this == NULL || pixmap() == None || (fMask == None && copyMask))
        return;

    int dim(horiz ? width() : height());
    if (dim >= 128) return;
    dim = 128 + dim - 128 % dim;

    Pixmap nPixmap(horiz ? createPixmap(dim, height())
                         : createPixmap(width(), dim));
    Pixmap nMask(copyMask ? (horiz ? createMask(dim, height())
                                   : createMask(width(), dim)) : None);

    if (horiz)
        Graphics(nPixmap, dim, height()).repHorz(fPixmap, width(), height(), 0, 0, dim);
    else
        Graphics(nPixmap, width(), dim).repVert(fPixmap, width(), height(), 0, 0, dim);

    if (nMask != None)
        if (horiz)
            Graphics(nMask, dim, height()).repHorz(fMask, width(), height(), 0, 0, dim);
        else
            Graphics(nMask, width(), dim).repVert(fMask, width(), height(), 0, 0, dim);

    if (fOwned) {
        if (fPixmap != None)
            XFreePixmap(xapp->display(), fPixmap);
        if (fMask != None)
            XFreePixmap(xapp->display(), fMask);
    }

    fPixmap = nPixmap;
    fMask = nMask;

    (horiz ? fWidth : fHeight) = dim;
}

ref<YPixmap> YPixmap::create(int w, int h, bool useMask) {
    ref<YPixmap> n;

    Pixmap pixmap = createPixmap(w, h);
    Pixmap mask = useMask ? createMask(w, h) : None;
    if (pixmap != None && (!useMask || mask != None)) {
        n.init(new YPixmap(pixmap, mask, w, h));
	n->fOwned = true;
    }
    return n;
}

ref<YPixmap> YPixmap::scale(ref<YPixmap> source, int const w, int const h) {
    ref<YPixmap> scaled;
#ifdef CONFIG_IMLIB
    if (source->width() != w || source->height() != h) {
        scaled.init(new YPixmap(source, w, h));
    } else
#endif
#endif
