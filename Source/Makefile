CC = psx-gcc
DEFINE= -DFIXMATH_FAST_SIN -D_PAL_MODE_ -DPSXSDK_DEBUG -DNO_CDDA -DNO_INTRO
LIBS= -lfixmath
CC_FLAGS = -Wall -Werror -c -Os
LINKER = psx-gcc

PROJECT = DEATHBALL
PROJECT_DIR = ~/DeathBall

ELF2EXE = elf2exe
ELF2EXE_FLAGS = -mark="A homebrew game created with PSXSDK"
LICENSE_FILE = /usr/local/psxsdk/share/licenses/infoeur.dat

PSXSDK_DIR = /usr/local/psxsdk/bin

EMULATOR_DIR = ~/pcsxr
EMULATOR = pcsxr.exe
SOUND_INTERFACE =
EMULATOR_FLAGS = -nogui -psxout
OBJ_DIR = Obj
SRC_DIR = .
#MUSIC_TRACKS = --track=../Bin/TRACK01.bin --track=../Bin/TRACK02.bin --track=../Bin/TRACK03.bin
FFMPEG = ffmpeg
FFMPEG_DIR = ../Music/ffmpeg/bin
FFMPEG_FLAGS = -f s16le -acodec pcm_s16le

GNU_SIZE = mipsel-unknown-elf-size

all: build image clean
#emulator clean

rebuild: remove build

build: clean objects $(PROJECT).elf $(PROJECT).exe
	
objects: 	$(addprefix $(OBJ_DIR)/,main.o System.o Menu.o Gfx.o Pad.o MainMenuBtnAni.o 	\
									LoadMenu.o GameGui.o Sfx.o Camera.o EndAnimation.o			\
									PSXSDKIntro.o Game.o Font.o MemCard.o)
			
#music_objects:	$(addprefix ../Music/, TRACK01.bin TRACK02.bin TRACK03.bin)
			
remove:
	rm -f Obj/*.o
	
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $< -o $@ $(DEFINE) $(CC_FLAGS)
	
$(PROJECT).elf: 
	$(LINKER) Obj/*.o -o Exe/$(PROJECT).elf $(LIBS)
	
$(PROJECT).exe:
	$(ELF2EXE) Exe/$(PROJECT).elf Exe/$(PROJECT).exe $(ELF2EXE_FLAGS)
	cp Exe/$(PROJECT).exe ../cdimg
	
image:
	rm -f $(PROJECT).iso $(PROJECT).bin
	rm -f $(PROJECT).cue
	mkisofs -o $(PROJECT).iso -V $(PROJECT) -sysid PLAYSTATION ../cdimg
	mkpsxiso $(PROJECT).iso $(PROJECT).bin $(LICENSE_FILE) $(MUSIC_TRACKS)
	mv $(PROJECT).bin ../Bin
	mv $(PROJECT).cue ../Bin
	rm -f $(PROJECT).cue
	rm -f $(PROJECT).iso
	$(GNU_SIZE) Exe/$(PROJECT).elf
	
emulator:
	export PATH=$$PATH:$(EMULATOR_DIR)
	$(EMULATOR) -cdfile $(PROJECT_DIR)/Bin/$(PROJECT).bin $(EMULATOR_FLAGS)
	
clean:
	rm -f $(PROJECT).elf cdimg/$(PROJECT).exe $(PROJECT).bin $(PROJECT).cue cdimg/README.txt
	rm -f $(PROJECT).iso $(PROJECT).exe $(PROJECT).elf

tiles:
	rm -f ../cdimg/DATA/SPRITES/*.TIM
	rm -f ../cdimg/DATA/FONTS/*.FNT
	bmp2tim ../Sprites/PSXDisk.bmp 		../cdimg/DATA/SPRITES/PSXDISK.TIM 16 -org=384,0 -mpink
	bmp2tim ../Sprites/IntroFnt.bmp 	../cdimg/DATA/SPRITES/INTROFNT.TIM 16 -org=384,76 -mpink
	bmp2tim ../Sprites/MainMenu.bmp 	../cdimg/DATA/SPRITES/MAINMENU.TIM 16 -org=768,384 -mpink
	bmp2tim ../Sprites/Loading.bmp 		../cdimg/DATA/SPRITES/LOADING.TIM 4 -mpink -org=768,256 -clut=384,496
	bmp2tim ../Sprites/Buttons.bmp 		../cdimg/DATA/SPRITES/BUTTONS.TIM 16 -org=784,288 -mpink
	bmp2tim ../Sprites/Font_1_4bit.bmp	../cdimg/DATA/FONTS/FONT_1.FNT 4 -org=960,256 -mpink -clut=384,500
	bmp2tim ../Sprites/Font_2_4bit.bmp	../cdimg/DATA/FONTS/FONT_2.FNT 4 -org=768,304 -mpink -clut=384,499
	bmp2tim ../Sprites/Arrows.bmp		../cdimg/DATA/SPRITES/ARROWS.TIM 16 -org=812,304 -mpink
	bmp2tim ../Sprites/GPL_4bit.bmp		../cdimg/DATA/SPRITES/GPL.TIM 4 -org=384,174 -mpink -clut=384,502
	bmp2tim ../Sprites/OpSrc_8bit.bmp	../cdimg/DATA/SPRITES/OPENSRC.TIM 8 -org=464,0 -mpink -clut=384,503
	bmp2tim ../Sprites/Mouse.bmp		../cdimg/DATA/SPRITES/MOUSE.TIM 16 -org=828,304 -mpink
	
sounds:
	rm -f ../cdimg/DATA/SOUNDS/*.VAG
	wav2vag ../Sounds/Bell.wav 		../cdimg/DATA/SOUNDS/BELL.VAG
	wav2vag ../Sounds/Accept.wav 	../cdimg/DATA/SOUNDS/ACCEPT.VAG
	wav2vag ../Sounds/TrayCl.wav 	../cdimg/DATA/SOUNDS/TRAYCL.VAG
	wav2vag ../Sounds/SpinDisk.wav	../cdimg/DATA/SOUNDS/SPINDISK.VAG

#music: clean_music music_objects

#clean_music:
#	rm -f ../Music/*.bin

%.bin: %.mp3
	rm -f ../Bin/$@1
	$(FFMPEG) -i $< $(FFMPEG_FLAGS) $@
	cp ../Music/$@ ../Bin/