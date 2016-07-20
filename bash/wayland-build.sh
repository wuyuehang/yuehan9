export WLD=$HOME/wbuild
export LD_LIBRARY_PATH=$WLD/lib
export PKG_CONFIG_PATH=$WLD/lib/pkgconfig/:$WLD/share/pkgconfig/
export PATH=$WLD/bin:$PATH
export ACLOCAL_PATH="$WLD/share/aclocal"
export ACLOCAL="aclocal -I $ACLOCAL_PATH"

REPOS="\
git://anongit.freedesktop.org/wayland/wayland \
git://anongit.freedesktop.org/git/mesa/drm \
git://anongit.freedesktop.org/xcb/proto \
git://anongit.freedesktop.org/xorg/util/macros \
git://anongit.freedesktop.org/xcb/libxcb \
git://anongit.freedesktop.org/xorg/proto/presentproto \
git://anongit.freedesktop.org/xorg/proto/dri3proto \
git://anongit.freedesktop.org/xorg/lib/libxshmfence \
git://github.com/xkbcommon/libxkbcommon \
git://anongit.freedesktop.org/mesa/mesa \
git://anongit.freedesktop.org/pixman \
git://anongit.freedesktop.org/cairo \
git://git.sv.gnu.org/libunwind \
git://anongit.freedesktop.org/libevdev \
git://anongit.freedesktop.org/wayland/wayland-protocols \
git://anongit.freedesktop.org/wayland/libinput \
git://anongit.freedesktop.org/wayland/weston"

deps()
{
		apt-get install -y git vim xmlto
		apt-get install -y zlib1g-dev libedit-dev
		apt-get install -y autoconf libtool libffi-dev
		apt-get install -y libexpat1-dev doxygen
		apt-get -y install graphviz libpng12-dev
		apt-get install -y xutils-dev flex
		apt-get install libpthread-stubs0-dev
		apt-get -y install libpciaccess-dev
		apt-get install -y libxau-dev bison libpam0g-dev
		apt-get -y install python-mako libudev-dev
		apt-get -y install x11proto-gl-dev libmtdev-dev libwacom-dev
		apt-get -y install x11proto-dri2-dev x11proto-randr-dev
		apt-get install -y libx11-dev libx11-xcb-dev
		apt-get install -y libxfixes-dev libxext-dev libxdamage-dev
		apt-get -y install libxcursor-dev libomxil-bellagio-dev

		is_trusty=$(grep 14.04 /etc/issue)

		if [ ! -z "${is_trusty}" ]; then
				apt-get install -y llvm-3.6-dev
				ln -sf llvm-config-3.6 /usr/bin/llvm-config
		else
				apt-get install -y llvm-3.7-dev
				ln -sf llvm-config-3.7 /usr/bin/llvm-config
		fi
}

init()
{
		cd $WLD/src
		for item in $REPOS; do
				repo_name=$(echo $item | rev | cut -d "/" -f 1 | rev)
				git clone $repo_name
		done
}

update_modules()
{
		cd $WLD/src
		for module in $(ls); do
				cd $module
				echo $module
				git pull || return $?
				cd ..
		done
}

build_path()
{
		if [ ! -e $WLD ]; then
				mkdir $WLD
		fi

		if [ ! -e $WLD/lib ]; then
				mkdir $WLD/lib
		fi

		if [ ! -e $WLD/bin ]; then
				mkdir $WLD/bin
		fi

		if [ ! -e $WLD/share ]; then
				mkdir $WLD/share
		fi

		if [ ! -e $WLD/lib/pkgconfig ]; then
				mkdir $WLD/lib/pkgconfig
		fi

		if [ ! -e $WLD/share/pkgconfig ]; then
				mkdir $WLD/share/pkgconfig
		fi

		if [ ! -e $WLD/share/aclocal ]; then
				mkdir $WLD/share/aclocal
		fi

		if [ ! -e $WLD/src ]; then
				mkdir $WLD/src
		fi
}

gen() {
		pkg=$1
		shift
		echo
		echo $pkg
		cd $WLD/src/$pkg
		echo "./autogen.sh --prefix=$WLD $*"
		./autogen.sh --prefix=$WLD $*
}

compile() {
		make -j32 && make install
		if [ $? != 0 ]; then
				echo "Build Error.  Terminating"
				exit
		fi
	}

build()
{
		gen wayland --disable-dtd-validation --disable-documentation
		compile

		gen wayland-protocols
		compile

		gen drm --disable-libkms
		compile

		gen proto
		compile

		gen macros
		compile

		gen libxcb
		compile

		gen presentproto
		compile

		gen dri3proto
		compile

		gen libxshmfence
		compile

		gen libxkbcommon --with-xkb-config-root=/usr/share/X11/xkb --disable-x11
		compile

		gen mesa --enable-debug --enable-gles2 --disable-gallium-egl \
				--with-egl-platforms=x11,wayland,drm --enable-gbm \
				--enable-shared-glapi --disable-llvm-shared-libs --disable-dri3 \
				--with-gallium-drivers=swrast,nouveau,r300,r600
		compile

		gen pixman
		compile

		gen cairo --enable-xcb --enable-gl --disable-full-testing
		compile

		gen libunwind --disable-documentation
		compile

		gen libevdev
		compile

		gen libinput --disable-libwacom --disable-tests --disable-documentation
		compile

		gen weston --with-cairo=image --enable-setuid-install=no \
				--enable-clients --enable-headless-compositor \
				--enable-demo-clients-install --disable-devdocs
		compile

		if [ -f $HOME/.bashrc ]; then
				temp=$(cat $HOME/.bashrc | grep -c 'XDG_RUNTIME_DIR')
				if [ -n "$temp" ]; then
						echo 'if test -z "$XDG_RUNTIME_DIR"; then' >> $HOME/.bashrc

						echo -e '\t\texport XDG_RUNTIME_DIR=/tmp/${UID}-runtime-dir' >> $HOME/.bashrc
						echo -e '\t\tif ! test -d "${XDG_RUNTIME_DIR}"; then' >> $HOME/.bashrc
						echo -e '\t\t\t\tmkdir "${XDG_RUNTIME_DIR}"' >> $HOME/.bashrc
						echo -e '\t\t\t\tchmod 0700 "${XDG_RUNTIME_DIR}"' >> $HOME/.bashrc
						echo -e '\t\tfi' >> $HOME/.bashrc
						echo fi >> $HOME/.bashrc
						echo 'export MESA_DEBUG=1' >> $HOME/.bashrc
						echo 'export EGL_LOG_LEVEL=debug' >> $HOME/.bashrc
						echo 'export LIBGL_DEBUG=verbose' >> $HOME/.bashrc
						echo 'export WAYLAND_DEBUG=1' >> $HOME/.bashrc
				fi
		fi

		if [ ! -d $HOME/.config ]; then
				mkdir $HOME/.config
		fi

		if [ ! -e $HOME/.config/weston.ini ]; then
				cp $WLD/src/weston/weston.ini $HOME/.config/
		fi
}

case "$1" in
		preinit)
				deps
				build_path
				;;
		init)
				init
				;;
		update)
				update_modules
				;;
		build)
				build
				;;
		*)
				echo "Usage: $0 preinit | init | update | build "
		exit 3
esac
