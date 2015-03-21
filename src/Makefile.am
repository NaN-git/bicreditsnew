DIST_SUBDIRS = secp256k1
AM_LDFLAGS = $(PTHREAD_CFLAGS) $(LIBTOOL_LDFLAGS)

if EMBEDDED_LEVELDB
LEVELDB_CPPFLAGS += -I$(srcdir)/leveldb/include
LEVELDB_CPPFLAGS += -I$(srcdir)/leveldb/helpers/memenv
LIBLEVELDB += $(builddir)/leveldb/libleveldb.a
LIBMEMENV  += $(builddir)/leveldb/libmemenv.a

# NOTE: This dependency is not strictly necessary, but without it make may try to build both in parallel, which breaks the LevelDB build system in a race
$(LIBLEVELDB): $(LIBMEMENV)

$(LIBLEVELDB) $(LIBMEMENV):
	@echo "Building LevelDB ..." && $(MAKE) -C $(@D) $(@F) CXX="$(CXX)" \
	  CC="$(CC)" PLATFORM=$(TARGET_OS) AR="$(AR)" $(LEVELDB_TARGET_FLAGS) \
          OPT="$(CXXFLAGS) $(CPPFLAGS)"
endif

BITCREDIT_CONFIG_INCLUDES=-I$(builddir)/config
BITCREDIT_INCLUDES=-I$(builddir) -I$(builddir)/obj $(BOOST_CPPFLAGS) $(LEVELDB_CPPFLAGS) $(CRYPTO_CFLAGS) $(SSL_CFLAGS)

BITCREDIT_INCLUDES += -I$(srcdir)/secp256k1/include

LIBBITCREDIT_SERVER=libbitcredit_server.a
LIBBITCREDIT_WALLET=libbitcredit_wallet.a
LIBBITCREDIT_COMMON=libbitcredit_common.a
LIBBITCREDIT_CLI=libbitcredit_cli.a
LIBBITCREDIT_UTIL=libbitcredit_util.a
LIBBITCREDIT_CRYPTO=crypto/libbitcredit_crypto.a
LIBBITCREDIT_UNIVALUE=univalue/libbitcredit_univalue.a
LIBBITCREDITQT=qt/libbitcreditqt.a
LIBSECP256K1=secp256k1/libsecp256k1.la

$(LIBSECP256K1): $(wildcard secp256k1/src/*) $(wildcard secp256k1/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)

# Make is not made aware of per-object dependencies to avoid limiting building parallelization
# But to build the less dependent modules first, we manually select their order here:
noinst_LIBRARIES = \
  crypto/libbitcredit_crypto.a \
  libbitcredit_util.a \
  libbitcredit_common.a \
  univalue/libbitcredit_univalue.a \
  libbitcredit_server.a \
  libbitcredit_cli.a
if ENABLE_WALLET
BITCREDIT_INCLUDES += $(BDB_CPPFLAGS)
noinst_LIBRARIES += libbitcredit_wallet.a
endif

if BUILD_BITCREDIT_LIBS
lib_LTLIBRARIES = libbitcreditconsensus.la
LIBBITCREDIT_CONSENSUS=libbitcreditconsensus.la
else
LIBBITCREDIT_CONSENSUS=
endif

bin_PROGRAMS =
TESTS =

if BUILD_BITCREDITD
  bin_PROGRAMS += bitcreditd
endif

if BUILD_BITCREDIT_UTILS
  bin_PROGRAMS += bitcredit-cli bitcredit-tx
endif

.PHONY: FORCE
# bitcredit core #
BITCREDIT_CORE_H = \
  activemasternode.h \
  addrman.h \
  alert.h \
  allocators.h \
  amount.h \
  base58.h \
  bloom.h \
  chain.h \
  chainparams.h \
  chainparamsbase.h \
  chainparamsseeds.h \
  checkpoints.h \
  checkqueue.h \
  clientversion.h \
  coincontrol.h \
  coins.h \
  compat.h \
  compressor.h \
  primitives/block.h \
  primitives/transaction.h \
  core_io.h \
  crypter.h \
  darksend.h \
  db.h \
  eccryptoverify.h \
  ecwrapper.h \
  hash.h \
  init.h \
  instantx.h \
  key.h \
  keepass.h \
  keystore.h \
  leveldbwrapper.h \
  limitedmap.h \
  lz4/lz4.h \
  main.h \
  masternode.h \
  masternodeconfig.h \
  merkleblock.h \
  miner.h \
  mruset.h \
  netbase.h \
  net.h \
  noui.h \
  pow.h \
  protocol.h \
  pubkey.h \
  random.h \
  rpcclient.h \
  rpcsmessage.h \
  rpcprotocol.h \
  rpcserver.h \
  script/interpreter.h \
  script/script.h \
  script/sigcache.h \
  script/sign.h \
  script/standard.h \
  script/script_error.h \
  serialize.h \
  smessage.h \
  streams.h \
  spork.h \
  sync.h \
  threadsafety.h \
  timedata.h \
  tinyformat.h \
  txdb.h \
  txmempool.h \
  ui_interface.h \
  uint256.h \
  undo.h \
  util.h \
  utilstrencodings.h \
  utilmoneystr.h \
  utiltime.h \
  version.h \
  wallet.h \
  momentum.h \
  semiOrderedMap.h \
  wallet_ismine.h \
  walletdb.h \
  irc.h \
  strlcpy.h \
  compat/sanity.h \
  xxhash/xxhash.h 

JSON_H = \
  json/json_spirit.h \
  json/json_spirit_error_position.h \
  json/json_spirit_reader.h \
  json/json_spirit_reader_template.h \
  json/json_spirit_stream_reader.h \
  json/json_spirit_utils.h \
  json/json_spirit_value.h \
  json/json_spirit_writer.h \
  json/json_spirit_writer_template.h

obj/build.h: FORCE
	@$(MKDIR_P) $(builddir)/obj
	@$(top_srcdir)/share/genbuild.sh $(abs_top_builddir)/src/obj/build.h \
	  $(abs_top_srcdir)
libbitcredit_util_a-clientversion.$(OBJEXT): obj/build.h

# server: shared between bitcreditd and bitcredit-qt
libbitcredit_server_a_CPPFLAGS = $(BITCREDIT_INCLUDES) $(MINIUPNPC_CPPFLAGS)
libbitcredit_server_a_SOURCES = \
  activemasternode.cpp \
  addrman.cpp \
  alert.cpp \
  bloom.cpp \
  chain.cpp \
  checkpoints.cpp \
  init.cpp \
  leveldbwrapper.cpp \
  lz4/lz4.c \
  main.cpp \
  merkleblock.cpp \
  miner.cpp \
  net.cpp \
  noui.cpp \
  pow.cpp \
  irc.cpp \
  rest.cpp \
  rpcblockchain.cpp \
  rpcdarksend.cpp \
  rpcsmessage.cpp \
  rpcmining.cpp \
  rpcmisc.cpp \
  rpcnet.cpp \
  rpcrawtransaction.cpp \
  rpcserver.cpp \
  script/sigcache.cpp \
  smessage.cpp \
  timedata.cpp \
  txdb.cpp \
  txmempool.cpp \
  xxhash/xxhash.c \
  $(JSON_H) \
  $(BITCREDIT_CORE_H)

# wallet: shared between bitcreditd and bitcredit-qt, but only linked
# when wallet enabled
libbitcredit_wallet_a_CPPFLAGS = $(BITCREDIT_INCLUDES)
libbitcredit_wallet_a_SOURCES = \
  db.cpp \
  crypter.cpp \
  rpcdump.cpp \
  rpcwallet.cpp \
  wallet.cpp \
  wallet_ismine.cpp \
  walletdb.cpp \
  keepass.cpp \
  $(BITCREDIT_CORE_H)

# crypto primitives library
crypto_libbitcredit_crypto_a_CPPFLAGS = $(BITCREDIT_CONFIG_INCLUDES)
crypto_libbitcredit_crypto_a_SOURCES = \
  crypto/sha1.cpp \
  crypto/sha256.cpp \
  crypto/sha512.cpp \
  crypto/hmac_sha256.cpp \
  crypto/rfc6979_hmac_sha256.cpp \
  crypto/hmac_sha512.cpp \
  crypto/ripemd160.cpp \
  crypto/common.h \
  crypto/sha256.h \
  crypto/sha512.h \
  crypto/hmac_sha256.h \
  crypto/rfc6979_hmac_sha256.h \
  crypto/hmac_sha512.h \
  crypto/sha1.h \
  crypto/ripemd160.h

# univalue JSON library
univalue_libbitcredit_univalue_a_SOURCES = \
  univalue/univalue.cpp \
  univalue/univalue_read.cpp \
  univalue/univalue_write.cpp \
  univalue/univalue_escapes.h \
  univalue/univalue.h

# common: shared between bitcreditd, and bitcredit-qt and non-server tools
libbitcredit_common_a_CPPFLAGS = $(BITCREDIT_INCLUDES)
libbitcredit_common_a_SOURCES = \
  allocators.cpp \
  amount.cpp \
  base58.cpp \
  chainparams.cpp \
  coins.cpp \
  compressor.cpp \
  darksend.cpp \
  masternode.cpp \
  masternodeconfig.cpp \
  instantx.cpp \
  momentum.cpp \
  primitives/block.cpp \
  primitives/transaction.cpp \
  core_read.cpp \
  core_write.cpp \
  eccryptoverify.cpp \
  ecwrapper.cpp \
  hash.cpp \
  key.cpp \
  keystore.cpp \
  netbase.cpp \
  protocol.cpp \
  pubkey.cpp \
  script/interpreter.cpp \
  script/script.cpp \
  script/sign.cpp \
  script/standard.cpp \
  script/script_error.cpp \
  $(BITCREDIT_CORE_H)

# util: shared between all executables.
# This library *must* be included to make sure that the glibc
# backward-compatibility objects and their sanity checks are linked.
libbitcredit_util_a_CPPFLAGS = $(BITCREDIT_INCLUDES)
libbitcredit_util_a_SOURCES = \
  compat/strnlen.cpp \
  compat/glibc_sanity.cpp \
  compat/glibcxx_sanity.cpp \
  chainparamsbase.cpp \
  clientversion.cpp \
  random.cpp \
  rpcprotocol.cpp \
  spork.cpp \
  sync.cpp \
  uint256.cpp \
  util.cpp \
  utilstrencodings.cpp \
  utilmoneystr.cpp \
  utiltime.cpp \
  $(BITCREDIT_CORE_H)

if GLIBC_BACK_COMPAT
libbitcredit_util_a_SOURCES += compat/glibc_compat.cpp
libbitcredit_util_a_SOURCES += compat/glibcxx_compat.cpp
endif

# cli: shared between bitcredit-cli and bitcredit-qt
libbitcredit_cli_a_CPPFLAGS = $(BITCREDIT_INCLUDES)
libbitcredit_cli_a_SOURCES = \
  rpcclient.cpp \
  $(BITCREDIT_CORE_H)

nodist_libbitcredit_util_a_SOURCES = $(srcdir)/obj/build.h
#

# bitcreditd binary #
bitcreditd_LDADD = \
  $(LIBBITCREDIT_SERVER) \
  $(LIBBITCREDIT_COMMON) \
  $(LIBBITCREDIT_UNIVALUE) \
  $(LIBBITCREDIT_UTIL) \
  $(LIBBITCREDIT_CRYPTO) \
  $(LIBLEVELDB) \
  $(LIBMEMENV) \
  $(LIBSECP256K1)

if ENABLE_WALLET
bitcreditd_LDADD += libbitcredit_wallet.a
endif
bitcreditd_SOURCES = bitcreditd.cpp
#

if TARGET_WINDOWS
bitcreditd_SOURCES += bitcreditd-res.rc
endif

bitcreditd_LDADD += $(BOOST_LIBS) $(BDB_LIBS) $(SSL_LIBS) $(CRYPTO_LIBS) $(MINIUPNPC_LIBS)
bitcreditd_CPPFLAGS = $(BITCREDIT_INCLUDES)
bitcreditd_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

# bitcredit-cli binary #
bitcredit_cli_LDADD = \
  $(LIBBITCREDIT_CLI) \
  $(LIBBITCREDIT_UTIL) \
  $(LIBBITCREDIT_COMMON) \
  $(BOOST_LIBS) \
  $(SSL_LIBS) \
  $(CRYPTO_LIBS)

bitcredit_cli_SOURCES = \
  bitcredit-cli.cpp

bitcredit_cli_CPPFLAGS = $(BITCREDIT_INCLUDES)
#

# bitcredit-tx binary #
bitcredit_tx_LDADD = \
  $(LIBBITCREDIT_UNIVALUE) \
  $(LIBBITCREDIT_COMMON) \
  $(LIBBITCREDIT_UTIL) \
  $(LIBBITCREDIT_CRYPTO) \
  $(LIBSECP256K1) \
  $(BOOST_LIBS) \
  $(CRYPTO_LIBS)

bitcredit_tx_SOURCES = bitcredit-tx.cpp
bitcredit_tx_CPPFLAGS = $(BITCREDIT_INCLUDES)
#
bitcredit_tx_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
bitcredit_cli_SOURCES += bitcredit-cli-res.rc
endif
bitcredit_cli_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if BUILD_BITCREDIT_LIBS
include_HEADERS = script/bitcreditconsensus.h
 
libbitcreditconsensus_la_SOURCES = \
  primitives/transaction.cpp \
  crypto/hmac_sha512.cpp \
  crypto/sha1.cpp \
  crypto/sha256.cpp \
  crypto/sha512.cpp \
  crypto/ripemd160.cpp \
  eccryptoverify.cpp \
  ecwrapper.cpp \
  hash.cpp \
  pubkey.cpp \
  script/script.cpp \
  script/interpreter.cpp \
  script/bitcreditconsensus.cpp \
  uint256.cpp \
  utilstrencodings.cpp

if GLIBC_BACK_COMPAT
  libbitcreditconsensus_la_SOURCES += compat/glibc_compat.cpp
  libbitcreditconsensus_la_SOURCES += compat/glibcxx_compat.cpp
endif

libbitcreditconsensus_la_LDFLAGS = -no-undefined $(RELDFLAGS)  
libbitcreditconsensus_la_LIBADD = $(LIBBITCREDIT_COMMON) $(CRYPTO_LIBS) 
libbitcreditconsensus_la_LIBADD += $(BOOST_LIBS)
libbitcreditconsensus_la_CPPFLAGS = $(CRYPTO_CFLAGS) -I$(builddir)/obj -DBUILD_BITCREDIT_INTERNAL
libbitcreditconsensus_la_CPPFLAGS += $(BITCREDIT_INCLUDES)
if USE_LIBSECP256K1
libbitcreditconsensus_la_LIBADD += secp256k1/libsecp256k1.la
endif
endif

CLEANFILES = leveldb/libleveldb.a leveldb/libmemenv.a *.gcda *.gcno

DISTCLEANFILES = obj/build.h

EXTRA_DIST = leveldb

clean-local:
	-$(MAKE) -C leveldb clean
	-$(MAKE) -C secp256k1 clean
	rm -f leveldb/*/*.gcno leveldb/helpers/memenv/*.gcno
	-rm -f config.h

.rc.o:
	@test -f $(WINDRES)
	$(AM_V_GEN) $(WINDRES) -DWINDRES_PREPROC -i $< -o $@

.mm.o:
	$(AM_V_CXX) $(OBJCXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	  $(CPPFLAGS) $(AM_CXXFLAGS) $(QT_INCLUDES) $(CXXFLAGS)  -c -o $@ $<

%.pb.cc %.pb.h: %.proto
	@test -f $(PROTOC)
	$(AM_V_GEN) $(PROTOC) --cpp_out=$(@D) --proto_path=$(abspath $(<D) $<)

if ENABLE_TESTS
include Makefile.test.include
endif

if ENABLE_QT
include Makefile.qt.include
endif

if ENABLE_QT_TESTS
include Makefile.qttest.include
endif
