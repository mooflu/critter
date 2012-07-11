bin_PROGRAMS = critter

critter_SOURCES = \
        ActionCallbacks.cpp \
        Callback.cpp \
        CallbackManager.cpp \
        EnemyFactory.cpp \
        Game.cpp \
        GameState.cpp \
        Hero.cpp \
        Input.cpp \
        Keys.cpp \
        LPathManager.cpp \
        MenuManager.cpp \
	OnlineUpdate.cpp \
        OnlineUpdateDisplay.cpp \
        ParticleGroup.cpp \
        ParticleGroupManager.cpp \
        ParticleType.cpp \
        Particles.cpp \
	ScoreKeeper.cpp \
	Selectable.cpp \
	SelectableFactory.cpp \
        BaseEnemy.cpp \
        SimpleEnemy.cpp \
        Boss1Enemy.cpp \
        Skill.cpp \
        StageManager.cpp \
        Starfield.cpp \
	TextInput.cpp \
        Video.cpp \
        Weapon.cpp \
        WeaponDepot.cpp \
        XMLHelper.cpp \
        main.cpp

LDADD = \
	../utils/libutils.a \
	../utilssdl/libutilssdl.a \
	../utilsgl/libutilsgl.a

INCLUDES = -I../utils -I../utilssdl -I../utilsgl

if WIN32
LDADD += critter.res
endif

depend:
	makedepend -I. -I/usr/include/g++-3 $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(AM_CXXFLAGS) $(critter_SOURCES)

if WIN32
else
pkgdata_DATA = resource.dat

resource.dat:
	../tools/Packer ../data/description_file resource.dat

DISTCLEANFILES = resource.dat critter-static
endif

small:
	strip $(bin_PROGRAMS)
	upx -9 $(bin_PROGRAMS)

critter-static: $(critter_OBJECTS) $(critter_DEPENDENCIES)
	@ln -sf `g++ -print-file-name=libstdc++.a`
	@rm -f critter-static
	$(CXXLINK) -static-libgcc -L. $(critter_LDFLAGS) $(critter_OBJECTS) $(critter_LDADD) $(LIBS)
	@rm -f libstdc++.a
	strip critter-static
	upx -9 critter-static


MAINTAINERCLEANFILES = Makefile.bak Makefile.in
