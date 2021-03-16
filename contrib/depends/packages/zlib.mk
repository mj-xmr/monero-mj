package=zlib
$(package)_version=1.2.11
$(package)_download_path=https://zlib.net
$(package)_file_name=zlib-$($(package)_version).tar.gz
$(package)_sha256_hash=c3e5e9fdd5004dcb542feda5ee4f0ff0744628baf8ed2dd5d66f8ca1197cb1a1

define $(package)_set_vars
$(package)_config_opts=--static
$(package)_config_opts+=--prefix=$(host_prefix)
endef

define $(package)_config_cmds
  CHOST=$(host) CC="$($(host_os)_CC) -target $(host) -fPIC -pipe" ARFLAGS=$($(host_os)_ARFLAGS) AR=$($(host_os)_AR) RANLIB=$($(host_os)_RANLIB) ./configure --static --prefix=$(host_prefix) --eprefix=$(host_prefix)
  #CHOST=$HOST ./configure $($(package)_config_opts) AR_FLAGS=$($(package)_arflags)
  #CHOST=$HOST ./configure $($(package)_config_opts)
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef

