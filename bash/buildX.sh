# !/bin/bash

## X.org development build script
## @author Łukasz Jernaś - original version
## @author legolas558 - some improvements
#

PREFIX="/opt/gfx-test"
PKG_CONFIG_PATH=/opt/gfx-test/lib/pkgconfig

# Attempt to detect proper concurrency level
CPU_CORES=`cat /proc/cpuinfo | grep -m1 "cpu cores" | sed s/".*: "//`
CONCURRENCY_LEVEL=$(( $CPU_CORES + 1 ))

MAKE="make"
MAKEINST="sudo make install"

## the freedesktop git server
GFDR="git://git.freedesktop.org/git"

REPOS="\
$GFDR/xorg/util/macros \
$GFDR/xorg/proto/x11proto \
$GFDR/xorg/proto/damageproto \
$GFDR/xorg/proto/xextproto \
$GFDR/xorg/proto/fontsproto \
$GFDR/xorg/proto/videoproto \
$GFDR/xorg/proto/renderproto \
$GFDR/xorg/proto/inputproto \
$GFDR/xorg/proto/xf86vidmodeproto \
$GFDR/xorg/proto/xf86dgaproto \
$GFDR/xorg/proto/xf86driproto \
$GFDR/xorg/proto/xcmiscproto \
$GFDR/xorg/proto/scrnsaverproto \
$GFDR/xorg/proto/bigreqsproto \
$GFDR/xorg/proto/resourceproto \
$GFDR/xorg/proto/compositeproto \
$GFDR/xorg/proto/fixesproto \
$GFDR/xorg/proto/evieproto \
$GFDR/xorg/proto/kbproto \
$GFDR/xorg/lib/libxtrans \
$GFDR/xorg/lib/libX11 \
$GFDR/xorg/lib/libXext \
$GFDR/xorg/lib/libXi \
$GFDR/xorg/lib/libxkbfile \
$GFDR/xorg/lib/libfontenc \
$GFDR/xorg/lib/libXfont \
$GFDR/xorg/lib/libXfixes \
$GFDR/xorg/lib/libXdamage \
$GFDR/xorg/lib/libXv \
$GFDR/xorg/lib/libXvMC \
$GFDR/xorg/lib/libXxf86vm \
$GFDR/xorg/lib/libXinerama \
$GFDR/xorg/proto/dri2proto \
$GFDR/xorg/proto/glproto \
$GFDR/xorg/lib/libpciaccess \
$GFDR/pixman \
$GFDR/xcb/proto \
$GFDR/xcb/pthread-stubs \
$GFDR/xorg/lib/libXau \
$GFDR/xcb/libxcb \
$GFDR/xorg/proto/randrproto \
$GFDR/mesa/drm \
$GFDR/mesa/mesa \
$GFDR/xorg/xserver \
$GFDR/xorg/driver/xf86-input-mouse \
$GFDR/xorg/driver/xf86-input-keyboard \
$GFDR/xorg/driver/xf86-video-intel \
$GFDR/xorg/driver/xf86-video-ati \
git://anongit.freedesktop.org/nouveau/xf86-video-nouveau \
git://anongit.freedesktop.org/xorg/driver/xf86-video-vmware \
$GFDR/xorg/driver/xf86-video-amdgpu"

modules="\
fontsproto \
x11proto \
xextproto \
videoproto \
renderproto \
inputproto \
damageproto \
xf86vidmodeproto \
xf86dgaproto \
xf86driproto \
xcmiscproto \
scrnsaverproto \
bigreqsproto \
resourceproto \
compositeproto \
resourceproto \
evieproto \
kbproto \
fixesproto \
proto \
pthread-stubs \
libxcb \
libXext \
libxtrans \
libX11 \
libXau \
libXi \
libxkbfile \
libfontenc \
libXfont \
libXv \
libXvMC \
libXxf86vm \
libXinerama \
libXfixes \
libXdamage \
dri2proto \
glproto \
libpciaccess \
pixman \
randrproto"

init()
{
        for repo in $REPOS; do
                echo "Cloning $repo"
                git clone $repo
        done
#        cd macros
#        echo "Building macros"
#        ./autogen.sh --prefix="$PREFIX"
#        ($MAKE) && \
#        $($MAKEINST)
#        cd ..
}

update_modules()
{
        for module in $REPOS; do
								repo_name=$(echo $module | rev | cut -d "/" -f 1 | rev)
                echo "Updating $repo_name"
                cd $repo_name
                git pull || return $?
                cd ..
        done
}

configure ()
{
        export ACLOCAL="aclocal -I $PREFIX/share/aclocal"
        export PKG_CONFIG_PATH="$PREFIX/lib/pkgconfig"
        ## configure all modules
        for i in $modules; do
                cd $i
                echo ======================
                echo configuring $i
                echo ======================
                ./autogen.sh --prefix="$PREFIX"
                if [ $? -ne 0 ]; then
                        echo "Failed to configure $i."
                        if [[ "$i" == "libX11" ]]; then
                                echo "If you are getting a libX11 error related to xtrans, install xtrans package into your system"
                        fi
                        exit -1
                fi
                cd ..

        done
        # build drm
        cd drm
        ./autogen.sh --prefix="$PREFIX"
        if [ $? -ne 0 ]; then
                echo "Failed to configure DRM."
                exit -2
        fi
        # assuming you're on Linux, otherwise use bsd-core
        cd ..
## configure mesa
        cd mesa
        ./autogen.sh --prefix=$PREFIX --with-driver=dri --disable-glut --with-state-trackers="egl dri"
        if [ $? -ne 0 ]; then
                echo "Failed to configure Mesa."
                exit -3
        fi
        cd ..
## configure xserver
        cd xserver
        ./autogen.sh --prefix=$PREFIX --enable-builtin-fonts
        if [ $? -ne 0 ]; then
                echo "Failed to configure X server."
                exit -4
        fi
        cd ..
## mouse, keyboard and intel video
        for MYM in "xf86-input-mouse xf86-input-keyboard xf86-video-intel"; do
                cd $MYM && \
                ./autogen.sh --prefix=$PREFIX
                if [ $? -ne 0 ]; then
                        echo "Failed to configure $MYM."
                        exit -5
                fi
                cd ..
        done
}

build ()
{
        export ACLOCAL="aclocal -I $PREFIX/share/aclocal"
        export PKG_CONFIG_PATH="$PREFIX/lib/pkgconfig"
        for i in $modules; do
                cd $i
                echo ======================
                echo building $i
                echo ======================
                ($MAKE) && \
                ($MAKEINST) || return $?
                cd ..

        done
        # build drm
        cd drm
        ## commented because not working - legolas558
        # assuming you're on Linux, otherwise use bsd-core
##        make -C linux-core
        ($MAKE) && \
        ($MAKEINST) || return $?
        cd ..
## build mesa
        cd mesa
        ($MAKE) && \
        ($MAKEINST) && \
        mkdir -p $PREFIX/bin && \
        sudo install -m755 progs/xdemos/{glxinfo,glxgears} $PREFIX/bin/ || return $?
        cd ..
## build xserver
        cd xserver
        ($MAKE) && \
        ($MAKEINST) && \
        sudo chown root $PREFIX/bin/Xorg && \
        sudo chmod +s $PREFIX/bin/Xorg || return $?
        cd ..
## build mouse, keyboard and intel video driver
        for MYM in "xf86-input-mouse xf86-input-keyboard xf86-video-intel"; do
                cd $MYM && \
                ($MAKE) && \
                ($MAKEINST)
                if [ $? -ne 0 ]; then
                        echo "Failed to build&install $MYM."
                        return -5
                fi
                cd ..
        done && \
        echo "All OK"
}

case "$1" in
        init)
                init
                ;;
        configure)
                configure
                ;;
        build)
                build
                ;;
        update)
                update_modules
                ;;
        *)
                echo "Usage: $0 init | update | configure | build "
                exit 3
esac
