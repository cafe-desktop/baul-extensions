AC_INIT([baul-extensions], 1.25.0, [https://github.com/cafe-desktop/baul-extensions])

AM_INIT_AUTOMAKE([1.9 no-dist-gzip dist-xz tar-ustar check-news])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
LT_INIT([disable-static])

BAUL_REQUIRED=1.21.3
LIBCAFE_DESKTOP_REQUIRED=1.17.0
GLIB_REQUIRED=2.50.0
CTK_REQUIRED=3.22.0

GLIB_GSETTINGS

GTK_DOC_CHECK(1.9)

CAFE_COMMON_INIT
CAFE_COMPILE_WARNINGS

PKG_PROG_PKG_CONFIG
# Give error and exit if we dont have pkgconfig
AS_IF([test "x$PKG_CONFIG" = "x"],[
  AC_MSG_ERROR([you need to have pkgconfig installed !])
])

GLIB_GENMARSHAL=`$PKG_CONFIG --variable=glib_genmarshal glib-2.0`
AC_SUBST(GLIB_GENMARSHAL)

# GLIB_COMPILE_RESOURCES

AC_ARG_VAR([GLIB_COMPILE_RESOURCES],[glib-compile-resources bin])
AC_PATH_PROG([GLIB_COMPILE_RESOURCES],[glib-compile-resources],[])
if test -z "$GLIB_COMPILE_RESOURCES"; then
  AC_MSG_ERROR([glib-compile-resources not found])
fi

# XMLLINT

AC_ARG_VAR([XMLLINT],[xmllint bin])
AC_PATH_PROG([XMLLINT],[xmllint],[])
if test -z "$XMLLINT"; then
  AC_MSG_ERROR([xmllint not found])
fi

# Common requirements
PKG_CHECK_MODULES(BAUL,
                  libbaul-extension >= $BAUL_REQUIRED)
AC_SUBST(BAUL_CFLAGS)
AC_SUBST(BAUL_LIBS)

# image-converter extension
AC_ARG_ENABLE([image-converter],
        AS_HELP_STRING([--enable-image-converter], [Enable image-converter plugin]),
        [enable_image_converter=$enableval],
        [enable_image_converter=yes])

AS_IF([test "x$enable_image-converter" = "xyes"],[
        PKG_CHECK_MODULES(IMAGE_CONVERTER,
                ctk+-3.0  >= $CTK_REQUIRED
                glib-2.0  >= $GLIB_REQUIRED
                gio-2.0   >= $GLIB_REQUIRED)
        AC_SUBST(IMAGE_CONVERTER_CFLAGS)
        AC_SUBST(IMAGE_CONVERTER_LIBS)
])

AM_CONDITIONAL(ENABLE_IMAGE_CONVERTER, test x"$enable_image_converter" = "xyes")

# open-terminal extension
AC_ARG_ENABLE([open-terminal],
        AS_HELP_STRING([--enable-open-terminal], [Enable open-terminal plugin]),
        [enable_open_terminal=$enableval],
        [enable_open_terminal=yes])

AS_IF([test "x$enable_open_terminal" = "xyes"],[
        PKG_CHECK_MODULES(OPENTERMINAL,
                glib-2.0           >= $GLIB_REQUIRED
                cafe-desktop-2.0   >= $LIBCAFE_DESKTOP_REQUIRED)
        AC_SUBST(OPENTERMINAL_CFLAGS)
        AC_SUBST(OPENTERMINAL_LIBS)
])

AM_CONDITIONAL(ENABLE_OPEN_TERMINAL, test x"$enable_open_terminal" = "xyes")

# sendto extension
AC_ARG_ENABLE([sendto],
        AS_HELP_STRING([--enable-sendto], [Enable sendto plugin]),
        [enable_sendto=$enableval],
        [enable_sendto=yes])

AS_IF([test "x$enable_sendto" = "xyes"],[
        GTHREAD_REQUIRED=2.6.0
        DBUS_REQUIRED=1.0
        DBUS_GLIB_REQUIRED=0.60
        GUPNP_REQUIRED=0.13
        PKG_CHECK_MODULES(SENDTO,
                glib-2.0    >= $GLIB_REQUIRED
                gthread-2.0 >= $GTHREAD_REQUIRED
                gmodule-2.0 >= $GLIB_REQUIRED
                gobject-2.0 >= $GLIB_REQUIRED
                ctk+-3.0 >= $CTK_REQUIRED)
        AC_SUBST(SENDTO_CFLAGS)
        AC_SUBST(SENDTO_LIBS)

    # The full list of plugins
    allowed_sendto_plugins="emailclient gajim baul-burn pidgin removable-devices upnp"

    sendto_plugin_error_or_ignore()
    {
        AS_IF([test "${error_on_bad_sendto_plugin}" = "1"],[
            AC_MSG_ERROR([$1])
        ],[
            AC_MSG_WARN([$1 (disabling sendto plugin)])
        ])
    }

    AC_ARG_WITH([sendto-plugins],
        [AS_HELP_STRING([--with-sendto-plugins],[Which baul-sendto plugins to compile (default: auto; "all", "autodetect" are valid)])],
        [], [with_sendto_plugins=autodetect])

    used_sendto_plugins=""
    used_sendto_plugins2=""

    AS_IF([test "x${with_sendto_plugins}" = "xautodetect"],[
        # Default to all plugins and just ignore them if they won't compile
        with_sendto_plugins="all"
        error_on_bad_sendto_plugin="0"
    ],[
        # Clean up the plugin list and error if a plugin won't compile
        with_sendto_plugins=`echo ,"${with_sendto_plugins}", | sed -e 's/[[ ,]][[ ,]]*/ /g' -e 's/,$//'`
        error_on_bad_sendto_plugin="1"
    ])

    # Iterate through the wanted plugins and weed out bad ones (or error)
    for wanted_sendto_plugin in ${with_sendto_plugins}; do
        for allowed_sendto_plugin in ${allowed_sendto_plugins}; do
            case ${wanted_sendto_plugin} in
		all)
                    # List all plugins as being used
                    used_sendto_plugins=${allowed_sendto_plugins}
                ;;
                autodetect)
                    # List all plugins as being used
                    used_sendto_plugins=${allowed_sendto_plugins}
                ;;
                ${allowed_sendto_plugin}) used_sendto_plugins="${used_sendto_plugins} ${wanted_sendto_plugin}";; # List the plugin
                *) ;;
            esac
        done
    done

    # Check for plugin-specific requirements and error if necessary
    for sendto_plugin in ${used_sendto_plugins}; do
        add_sendto_plugin="1"

        case ${sendto_plugin} in
            emailclient)
                PKG_CHECK_MODULES(EMAILCLIENT, gio-2.0,
                    enable_emailclient=yes, enable_emailclient=no)
                if test "${enable_emailclient}" != "yes" ; then
                    sendto_plugin_error_or_ignore "you need gio-2.0 to build the emailclient plugin"
                    add_sendto_plugin="0"
                fi
            ;;
            gajim)
                AC_ARG_WITH(sendto_gajim,
                    [AS_HELP_STRING(--with-sendto-gajim,[Path to Gajim install prefix])],
                    [GAJIM_PATH="$withval"],[GAJIM_PATH="$prefix"])

                GAJIM_SHARE_DIR="$GAJIM_PATH"/share/gajim

                PKG_CHECK_MODULES(DBUS, dbus-1 >= $DBUS_REQUIRED dbus-glib-1 >= $DBUS_GLIB_REQUIRED,
                    [AC_SUBST(DBUS_CFLAGS)
                    AC_SUBST(DBUS_LIBS)
                    enable_gajim=yes
                    AC_DEFINE_UNQUOTED(GAJIM_SHARE_DIR, "$GAJIM_SHARE_DIR", [path to gajim share dir])],
                    [enable_gajim=no])

                if test "${enable_gajim}" != "yes" ; then
                    sendto_plugin_error_or_ignore "you need dbus-glib installed to build the gajim plugin"
                    add_sendto_plugin="0"
                fi
            ;;
            baul-burn)
                PKG_CHECK_MODULES(GIO, gio-2.0,
                    enable_burn=yes, enable_burn=no)
                if test "${enable_burn}" != "yes" ; then
                    sendto_plugin_error_or_ignore "you need gio installed to build the baul-burn plugin"
                    add_sendto_plugin="0"
                fi
            ;;
            pidgin)
                PKG_CHECK_MODULES(PIDGIN, dbus-glib-1 >= $DBUS_GLIB_REQUIRED,
                    enable_pidgin=yes, enable_pidgin=no)
                if test "${enable_pidgin}" != "yes" ; then
                    sendto_plugin_error_or_ignore "you need dbus-glib installed to build the pidgin plugin"
                    add_sendto_plugin="0"
                fi
            ;;
            removable-devices)
                PKG_CHECK_MODULES(GIO, gio-2.0,
                    enable_removable=yes, enable_removable=no)
                if test "${enable_removable}" != "yes" ; then
                    sendto_plugin_error_or_ignore "you need gio-2.0 installed to build the removable-devices plugin"
                    add_sendto_plugin="0"
            fi
            ;;
            upnp)
                PKG_CHECK_MODULES(UPNP, gupnp-1.2 >= $GUPNP_REQUIRED,
                    enable_upnp=yes AC_DEFINE([HAS_GUPNP_VERSION_1_2], 1, ["Version is at least 1.2"]), enable_upnp=no)
                PKG_CHECK_MODULES(UPNP, gupnp-1.0 >= $GUPNP_REQUIRED,
                    enable_upnp=yes, enable_upnp=no)
                if test "${enable_upnp}" != "yes" ; then
                    sendto_plugin_error_or_ignore "you need gupnp installed to build the upnp plugin"
                    add_sendto_plugin="0"
                fi
            ;;
        esac

        # Add the specified plugin
        if test "${add_sendto_plugin}" = "1" ; then
            used_sendto_plugins2="${used_sendto_plugins2} ${sendto_plugin}"
        fi
    done

    ALL_SENDTO_PLUGINS=$allowed_sendto_plugins
    SENDTO_PLUGINS=$used_sendto_plugins2

    dnl Error out if no plugins will be built
    AS_IF([test -z "${SENDTO_PLUGINS}"],[
        AC_MSG_ERROR([No sendto plugins will be built, and baul-sendto requires at least one])
    ])

    AC_MSG_CHECKING([which sendto plugins to compile])
    AC_MSG_RESULT([$SENDTO_PLUGINS])

    AC_SUBST([ALL_SENDTO_PLUGINS])
    AC_SUBST([SENDTO_PLUGINS])

    dnl For the pidgin sendto plugin
    AM_CONDITIONAL(HAVE_SENDTO_PIDGIN, test "x$enable_pidgin" = "xyes")

    dnl For the GIO based plugins
    enable_nst_common=no
    AS_IF([test "x$enable_burn" = "xyes" -o "x$enable_removable" = "xyes"],[
        enable_nst_common=yes
    ])
    AM_CONDITIONAL(HAVE_GIO, test "x$enable_nst_common" = "xyes")

    AS_IF([test "x${SENDTO_PLUGINS}" != "x"],[
        for allowed_sendto_plugin in ${ALL_SENDTO_PLUGINS}; do
            for sendto_plugin in ${SENDTO_PLUGINS}; do
                case ${allowed_sendto_plugin} in
                    ${sendto_plugin})
                        AC_MSG_NOTICE([** ${allowed_sendto_plugin} sendto plugin enabled])
                        continue 2
                    ;;
                    *);;
                esac
            done
            AC_MSG_NOTICE([ ${allowed_sendto_plugin} sendto plugin disabled])
        done
    ],[
        AC_MSG_NOTICE([ No baul-sendto plugins enabled])
    ])
])
AM_CONDITIONAL(ENABLE_SENDTO, test x"$enable_sendto" = "xyes")

# share extension
AC_ARG_ENABLE([share],
        AS_HELP_STRING([--enable-share], [Enable share plugin]),
        [enable_share=$enableval],
        [enable_share=yes])

AS_IF([test "x$enable_share" = "xyes"],[
        PKG_CHECK_MODULES(SHARE,
                glib-2.0 >= $GLIB_REQUIRED)
        AC_SUBST(SHARE_CFLAGS)
        AC_SUBST(SHARE_LIBS)
])

AM_CONDITIONAL(ENABLE_SHARE, test x"$enable_share" = "xyes")

# gksu extension
AC_ARG_ENABLE([gksu],
        AS_HELP_STRING([--enable-gksu], [Enable gksu plugin]),
        [enable_gksu=$enableval],
        [enable_gksu=yes])

AM_CONDITIONAL(ENABLE_GKSU, test x"$enable_gksu" = "xyes")

# wallpaper extension
AC_ARG_ENABLE([wallpaper],
        AS_HELP_STRING([--enable-wallpaper], [Enable set as wallpaper plugin]),
        [enable_wallpaper=$enableval],
        [enable_wallpaper=yes])

AM_CONDITIONAL(ENABLE_WALLPAPER, test x"$enable_wallpaper" = "xyes")

# xattr-tags extension
AC_ARG_ENABLE([xattr-tags],
        AS_HELP_STRING([--enable-xattr-tags], [Enable xattr tags plugin]),
        [enable_xattr_tags=$enableval],
        [enable_xattr_tags=yes])

AM_CONDITIONAL(ENABLE_XATTR_TAGS, test x"$enable_xattr_tags" = "xyes")

# Get baul extensions directory
AC_ARG_WITH(bauldir,
    AS_HELP_STRING([--with-bauldir=DIR],[Installation path for Baul extension @<:@auto@:>@]),
    [ac_with_bauldir=$withval],
    [ac_with_bauldir=""])
AS_IF([test "${ac_with_bauldir}" = ""],[
    ac_with_bauldir=`$PKG_CONFIG --variable=extensiondir libbaul-extension`
])
AC_MSG_NOTICE([Installing baul plugins in ${ac_with_bauldir}])
AC_SUBST([BAUL_EXTENSION_DIR],[${ac_with_bauldir}])

# gettext
AM_GNU_GETTEXT_VERSION([0.19.8])
AM_GNU_GETTEXT([external])

GETTEXT_PACKAGE=baul-extensions
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [gettext package])

AC_CONFIG_FILES([
  Makefile
  sendto/org.cafe.Baul.Sendto.gschema.xml
  open-terminal/org.cafe.baul-open-terminal.gschema.xml
  gksu/libbaul-gksu.baul-extension.desktop.in
  gksu/Makefile
  image-converter/libbaul-image-converter.baul-extension.desktop.in
  image-converter/Makefile
  open-terminal/libbaul-open-terminal.baul-extension.desktop.in
  open-terminal/Makefile
  sendto/libbaul-sendto.baul-extension.desktop.in
  sendto/Makefile
  sendto/baul-sendto.pc
  sendto/docs/Makefile
  sendto/docs/version.xml
  sendto/plugins/Makefile
  sendto/plugins/emailclient/Makefile
  sendto/plugins/gajim/Makefile
  sendto/plugins/baul-burn/Makefile
  sendto/plugins/pidgin/Makefile
  sendto/plugins/removable-devices/Makefile
  sendto/plugins/upnp/Makefile
  share/libbaul-share.baul-extension.desktop.in
  share/Makefile
  wallpaper/libbaul-wallpaper.baul-extension.desktop.in
  wallpaper/Makefile
  xattr-tags/libbaul-xattr-tags.baul-extension.desktop.in
  xattr-tags/Makefile
  po/Makefile.in
])

AC_OUTPUT

echo "
      baul-extensions $VERSION
      `echo baul-extensions $VERSION | sed "s/./=/g"`
      prefix:                 ${prefix}
      compiler:               ${CC}
      cflags:                 ${CFLAGS}
      warning flags:          ${WARN_CFLAGS}
      baul-extension dir      ${ac_with_bauldir}

      Plugins to be built:
      Image Converter:        $enable_image_converter
      Open Terminal:          $enable_open_terminal
      Sendto:                 $enable_sendto
      Share:                  $enable_share
      Gksu:                   $enable_gksu
      Wallpaper:              $enable_wallpaper
      xattr Tags:             $enable_xattr_tags
"
