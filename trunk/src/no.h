/****************************************************************************
*  Copyright (c) 2013 by Michael Blandford. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*  1. Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*  3. Neither the name of the author nor the names of its contributors may
*     be used to endorse or promote products derived from this software
*     without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
*  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
*  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
*  SUCH DAMAGE.
*
****************************************************************************
* Other Authors:
 * - Torodd Indersund - main translator
 * - Andre Bernet
 * - Bertrand Songis
 * - Bryan J. Rentoul (Gruvin)
 * - Cameron Weeks
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini
 * - Thomas Husterer
*
****************************************************************************/


#define STR_ON             "P� "
#define STR_OFF            "Av"

#define STR_ALTEQ	         "Alt=" 
#define STR_TXEQ		       "Tx="
#define STR_RXEQ		       "Rx="
#define STR_TRE012AG	     "TRE012AG"

// STR_YELORGRED indexed 3 char each
#define STR_YELORGRED	     "\003---YelOrgRed"
#define STR_A_EQ		       "A ="
#define STR_SOUNDS	       "\006Warn1 ""Warn2 ""Cheap ""Ring  ""SciFi ""Robot ""Chirp ""Tada  ""Crickt""Siren ""AlmClk""Ratata""Tick  ""Haptc1""Haptc2""Haptc3"
#define STR_SWITCH_WARN	   "Bryter Advarsel"
// STR_TIMER exactly 5 chars long
#define STR_TIMER          "Timer"

// STR_PPMCHANNELS indexed 4 char each
#define STR_PPMCHANNELS	   "\0044CH 6CH 8CH 10CH12CH14CH16CH"

#define STR_MAH_ALARM      "mAh Alarm"


// er9x.cpp
// ********
#define STR_LIMITS		     "LIMITS"
#define STR_EE_LOW_MEM     "EEPROM lavt minne"
#define STR_ALERT		       " ALERT"
#define STR_THR_NOT_IDLE   "Gasspake ikke i null"
#define STR_RST_THROTTLE   "Nullstill gasspake"
#define STR_PRESS_KEY_SKIP "Trykk en knapp for � skippe"
#define STR_ALARMS_DISABLE "Alarmer deaktivert"
#define STR_OLD_VER_EEPROM "Gammel version EEPROM  Sjekk innst/rekalib"
#define STR_RESET_SWITCHES "Nullstill brytere"
#define STR_LOADING        "LASTER"
#define STR_MESSAGE        "MELDING"
#define STR_PRESS_ANY_KEY  "Trykk en knapp"
#define STR_MSTACK_UFLOW   "mStack uflow"
#define STR_MSTACK_OFLOW   "mStack oflow"

#define STR_CHANS_GV	     "\004P1  P2  P3  HALVFULLCYC1CYC2CYC3PPM1PPM2PPM3PPM4PPM5PPM6PPM7PPM8CH1 CH2 CH3 CH4 CH5 CH6 CH7 CH8 CH9 CH10CH11CH12CH13CH14CH15CH163POSGV1 GV2 GV3 GV4 GV5 GV6 GV7 THIS"
#define STR_CHANS_RAW	     "\004P1  P2  P3  HALVFULLCYC1CYC2CYC3PPM1PPM2PPM3PPM4PPM5PPM6PPM7PPM8CH1 CH2 CH3 CH4 CH5 CH6 CH7 CH8 CH9 CH10CH11CH12CH13CH14CH15CH163POS"
#define STR_CH	           "CH"
#define STR_TMR_MODE	     "\003OFFABSRUsRU%ELsEL%THsTH%ALsAL%P1 P1%P2 P2%P3 P3%"

// pers.cpp
// ********
#define STR_ME             "MEG       "
#define STR_MODEL          "MODELL    "
#define STR_BAD_EEPROM     "D�rlig EEprom Data"
#define STR_EE_FORMAT      "EEPROM Formaterer"
#define STR_GENWR_ERROR    "genwrite error"
#define STR_EE_OFLOW       "EEPROM overflow"

// templates.cpp
// ***********
#define STR_T_S_4CHAN      "Simple 4-CH"
#define STR_T_TCUT         "T-Cut"
#define STR_T_STICK_TCUT   "Sticky T-Cut"
#define STR_T_V_TAIL       "V-Tail"
#define STR_T_ELEVON       "Elevon\\Delta"
#define STR_T_HELI_SETUP   "Heli Setup"
#define STR_T_GYRO         "Gyro Setup"
#define STR_T_SERVO_TEST   "Servo Test"

// menus.cpp
// ***********
#define STR_TELEM_ITEMS	   "\004----A1= A2= RSSITSSITim1Tim2Alt GaltGspdT1= T2= RPM FUELMah1Mah2CvltBattAmpsMah CtotFasVAccXAccYAccZVspdGvr1Gvr2Gvr3Gvr4Gvr5Gvr6Gvr7FwatRxV Hdg A3= A4= SC1 SC2 SC3 SC4 "
#define STR_TELEM_SHORT    "\004----TIM1TIM2BATTGvr1Gvr2Gvr3Gvr4Gvr5Gvr6Gvr7"
#define STR_GV             "GV"
#define STR_OFF_ON         "av p� "
#define STR_HYPH_INV       "\003---INV"
#define STR_VERSION        "VERSION"
#define STR_TRAINER        "TRAINER"
#define STR_SLAVE          "\007Slave" 
#define STR_MENU_DONE      "[MENU] N�r du er ferdig"
#define STR_CURVES         "KURVER"
#define STR_CURVE          "KURVE"
#define STR_GLOBAL_VAR     "GLOBAL VAR"
#define STR_VALUE          "Verdi"
#define STR_PRESET         "PRESET"
#define STR_CV             "CV"
#define STR_LIMITS         "LIMITS"
#define STR_COPY_TRIM      "Kopier TRIM [MENU]"
#define STR_TELEMETRY      "TELEMETRY"
#define STR_USR_PROTO      "UsrProto"
#define STR_FRHUB_WSHHI    "\005FrHubWSHhi"
#define STR_MET_IMP        "\003MetImp"
#define STR_A_CHANNEL      "A  channel"
#define STR_ALRM           "alrm"
#define STR_TELEMETRY2     "TELEMETRY2"
#define STR_TX_RSSIALRM    "TxRSSIalrm"
#define STR_NUM_BLADES     "Num Blades"
#if ALT_ALARM
#define STR_ALT_ALARM      "AltAlarm"
#define STR_OFF122400      "\003OFF122400"
#endif
#define STR_VOLT_THRES     "Volt Thres="
#define STR_GPS_ALTMAIN    "GpsAltMain"
#define STR_CUSTOM_DISP    "Custom Display"
#define STR_FAS_OFFSET     "FAS Offset"
#define STR_VARIO_SRC      "Vario: Source"
#define STR_VSPD_A2        "\004----vspdA2  "
#define STR_2SWITCH        "\002Bryter"
#define STR_2SENSITIVITY   "\002F�lsomhet"
#define STR_GLOBAL_VARS    "GLOBAL VARS"
#define STR_GV_SOURCE      "\003---RtmEtmTtmAtmRENRUDELETHRAILP1 P2 P3 c1 c2 c3 c4 c5 c6 c7 c8 c9 c10c11c12c13c14c15c16"
#define STR_TEMPLATES      "Maler"
#define STR_CHAN_ORDER     "\001Kanal rekkef�lge"
#define STR_SP_RETA        " RETA"
#define STR_CLEAR_MIXES    "Fjern Mixer [MENU]"
#define STR_SAFETY_SW      "SIKKERHETS BRYTERE"
#define STR_NUM_VOICE_SW   "Tall stemme SW"
#define STR_V_OPT1         "\007 8 Secs12 Secs16 Secs"
#define STR_VS             "VS"
#define STR_VOICE_OPT      "\006P�    AV    BEGGE 15Secs30Secs60SecsVaribl"
#define STR_CUST_SWITCH    "CUSTOM BRYTERE"
//#define STR_S              "S"
#define STR_15_ON          "\015P�"
#define STR_EDIT_MIX       "Rediger MIX "
#define STR_2SOURCE        "\002Kilde"
#define STR_2WEIGHT        "\002Vekt"
#ifdef FMODE_TRIM
#define STR_FMTRIMVAL      "FmTrimVal"
#endif
#define STR_OFFSET         "Offset"
#define STR_2FIX_OFFSET    "\002Fix Offset"
#define STR_FLMODETRIM     "\002FlModetrim"
#define STR_2TRIM          "\002Trim"
#define STR_15DIFF         "\015Diff"
#define STR_Curve          "Kurve"
#define STR_2WARNING       "\002Advarsel"
#define STR_2MULTIPLEX     "\002Multpx"
// STR_ADD_MULT_REP indexed 8 chars each
#define STR_ADD_MULT_REP   "\010Legg tilGang medErstatt "
#define STR_2DELAY_DOWN    "\002Delay Ned"
#define STR_2DELAY_UP      "\002Delay Opp"
#define STR_2SLOW_DOWN     "\002Treg  Ned"
#define STR_2SLOW_UP       "\002Treg  Opp"
#define STR_MAX_MIXERS     "Max mixers n�dd: 32"
#define STR_PRESS_EXIT_AB  "Trykk [EXIT] for � avbryte"
#define STR_YES_NO         "\003JA \013NEI"
#define STR_MENU_EXIT      "\003[MENU]\013[EXIT]"
#define STR_DELETE_MIX     "SLETTE MIX?"
#define STR_MIX_POPUP      "Rediger\0Legg til\0Kopier\0Flytt\0Slette"
#define STR_MIXER          "MIXER"
// CHR_S S for Slow
#define CHR_S              'T'
// CHR_D D for Delay
#define CHR_D              'D'
// CHR_d d for differential
#define CHR_d              'd'
#define STR_EXPO_DR        "EXPO/DR"
#define STR_4DR_MID        "\004DR Med"
#define STR_4DR_LOW        "\004DR Lav"
#define STR_4DR_HI         "\004DR H�y"
#define STR_2EXPO          "\002Expo"
#define STR_DR_SW1         "DrSw1"
#define STR_DR_SW2         "DrSw2"
#define STR_DUP_MODEL      "Duplisere modell"
#define STR_DELETE_MODEL   "SLETTE MODELL"
#define STR_DUPLICATING    "Dupliserer modell"
#define STR_SETUP          "SETUP"
#define STR_NAME           "Navn"
#define STR_VOICE_INDEX    "Stemme Index\021MENU"
#define STR_TRIGGER        "Trigger"
#define STR_TRIGGERB       "TriggerB"
//STR_COUNT_DOWN_UP indexed, 10 chars each
#define STR_COUNT_DOWN_UP  "\012Tell Ned  Tell opp  "
#define STR_T_TRIM         "T-Trim"
#define STR_T_EXPO         "T-Expo"
#define STR_TRIM_INC       "Trim steg"
// STR_TRIM_OPTIONS indexed 6 chars each
#define STR_TRIM_OPTIONS   "\006Exp   ExFin Fin   MediumHard  "
#define STR_TRIM_SWITCH    "Trim Br"
#define STR_BEEP_CENTRE    "Beep Snt"
#define STR_RETA123        "RETA123"
#define STR_PROTO          "Proto"
// STR_21_USEC after \021 max 4 chars
#define STR_21_USEC        "\021uSec"
#define STR_13_RXNUM       "\013RxNum"
// STR_23_US after \023 max 2 chars
#define STR_23_US          "\023uS"
// STR_PPMFRAME_MSEC before \015 max 9 chars, after max 4 chars
#define STR_PPMFRAME_MSEC  "PPM FrLen\015mSec"
#define STR_SEND_RX_NUM    "Bind  Range"
#define STR_DSM_TYPE       "DSM Type"
#define STR_PPM_1ST_CHAN   "PPM 1st Kanal"
#define STR_SHIFT_SEL      "Shift Sel"
// STR_POS_NEG indexed 3 chars each
#define STR_POS_NEG        "\003POSNEG"
#define STR_E_LIMITS       "E. Limits"
#define STR_Trainer        "Trener"
#define STR_T2THTRIG       "T2ThTrig"
#define STR_AUTO_LIMITS    "Auto Limits"
// STR_1_RETA indexed 1 char each
#define STR_1_RETA         "\001RETA"
#define STR_FL_MODE        "FL MODE"
#define STR_SWITCH         "Brytere"
#define STR_TRIMS          "Trims"
#define STR_MODES          "MODUSER"
#define STR_SP_FM0         " FM0"
#define STR_SP_FM          " FM"
#define STR_HELI_SETUP     "HELI SETUP"
#define STR_SWASH_TYPE     "Swash Type"
#define STR_COLLECTIVE     "Collective"
#define STR_SWASH_RING     "Swash Ring"
#define STR_ELE_DIRECTION  "ELE Direction"
#define STR_AIL_DIRECTION  "AIL Direction"
#define STR_COL_DIRECTION  "COL Direction"
#define STR_MODEL_POPUP    "Velg\0Kopier\0Flytt\0SLETTE"
#define STR_MODELSEL       "MODELSEL"
// STR_11_FREE after \011 max 4 chars
#define STR_11_FREE        "\011free"
#define STR_CALIBRATION    "Kalibrering"
// STR_MENU_TO_START after \003 max 15 chars
#define STR_MENU_TO_START  "\003[MENU]For start"
// STR_SET_MIDPOINT after \005 max 11 chars
#define STR_SET_MIDPOINT   "\005SET MIDPOINT"
// STR_MOVE_STICKS after \003 max 15 chars
#define STR_MOVE_STICKS    "\003BEVEG SPAK/POTS"
#define STR_ANA            "ANA"
#define STR_DIAG           "DIAG"
// STR_KEYNAMES indexed 5 chars each
#define STR_KEYNAMES       "\005 Menu Exit  Ned  OppH�yreVenst"
#define STR_TRIM_M_P       "Trim- +"
// STR_OFF_PLUS_EQ indexed 3 chars each
#define STR_OFF_PLUS_EQ    "\003off += :="
// STR_CH1_4 indexed 3 chars each
#define STR_CH1_4          "\003ch1ch2ch3ch4"
#define STR_MULTIPLIER     "Multiplier"
#define STR_CAL            "Cal"
#define STR_MODE_SRC_SW    "\003mode\012% src  sw"
#define STR_RADIO_SETUP    "General"
#define STR_OWNER_NAME     "Eiers navn"
#define STR_BEEPER         "Beeper"
// STR_BEEP_MODES indexed 6 chars each
#define STR_BEEP_MODES     "\006Stille""NoKey ""xKort ""Kort ""Norm  ""Lang  ""xLang "
#define STR_SOUND_MODE     "Lyd Modus"
// STR_SPEAKER_OPTS indexed 10 chars each
#define STR_SPEAKER_OPTS   "\012Beeper    ""PiSpkr    ""BeeprVoice""PiSpkVoice""MegaSound "
#define STR_VOLUME         " Volum"
#define STR_SPEAKER_PITCH  " Speaker Pitch"
#define STR_HAPTICSTRENGTH " Viberator styrke"
#define STR_CONTRAST       "Kontrast"
#define STR_BATT_WARN      "Batteri alarm" 
// STR_INACT_ALARM m for minutes after \023 - single char
#define STR_INACT_ALARM    "Inaktivitets alarm\023m"
#define STR_THR_REVERSE    "Gasspak reversert"
#define STR_MINUTE_BEEP    "Minutt beep"
#define STR_BEEP_COUNTDOWN "Beep nedtelling"
#define STR_FLASH_ON_BEEP  "Blink p� beep"
#define STR_LIGHT_SWITCH   "Lys bryter"
#define STR_LIGHT_INVERT   "Baklys invertert"
#define STR_LIGHT_AFTER    "Lys av etter"
#define STR_LIGHT_STICK    "Lys p� Stk Mv"
#define STR_SPLASH_SCREEN  "Oppstart skjerm"
#define STR_SPLASH_NAME    "Oppstart navn"
#define STR_THR_WARNING    "Gasspak advarsel"
#define STR_DEAFULT_SW     "Standard Br"
#define STR_MEM_WARN       "Minne advarsel"
#define STR_ALARM_WARN     "Alarm advarsel"
#define STR_POTSCROLL      "PotScroll"
#define STR_STICKSCROLL    "StickScroll"
#define STR_BANDGAP        "BandGap"
#define STR_ENABLE_PPMSIM  "Sl� p� PPMSIM"
#define STR_CROSSTRIM      "CrossTrim"
#define STR_INT_FRSKY_ALRM "Int. Frsky alarm"
#define STR_MODE           "Modus"

// SWITCHES_STR 3 chars each
#if defined(CPUM128) || defined(CPUM2561)
#define SWITCHES_STR       "\003THR""RUD""ELE""ID0""ID1""ID2""AIL""GEA""TRN""SW1""SW2""SW3""SW4""SW5""SW6""SW7""SW8""SW9""SWA""SWB""SWCSWDSWESWFSWGSWHSWI"
#else
#define SWITCHES_STR       "\003THR""RUD""ELE""ID0""ID1""ID2""AIL""GEA""TRN""SW1""SW2""SW3""SW4""SW5""SW6""SW7""SW8""SW9""SWA""SWB""SWC"
#endif
#define SWITCH_WARN_STR	   "Bryter advarsel"
// CURV_STR indexed 3 chars each
#define CURV_STR           "\003---x>0x<0|x|f>0f<0|f|c1 c2 c3 c4 c5 c6 c7 c8 c9 c10c11c12c13c14c15c16"
// CSWITCH_STR indexed 7 chars each
#ifdef VERSION3
#define CSWITCH_STR        "\007----   v>ofs  v<ofs  |v|>ofs|v|<ofsAND    OR     XOR    ""v1==v2 ""v1!=v2 ""v1>v2  ""v1<v2  ""Latch  ""F-Flop TimeOff"
#else
#define CSWITCH_STR        "\007----   v>ofs  v<ofs  |v|>ofs|v|<ofsAND    OR     XOR    ""v1==v2 ""v1!=v2 ""v1>v2  ""v1<v2  ""v1>=v2 ""v1<=v2 TimeOff"
#endif

#define SWASH_TYPE_STR     "\006---   ""120   ""120X  ""140   ""90    "

#define STR_STICK_NAMES    "RUD ELE THR AIL "

#define STR_STAT           "STAT"
#define STR_STAT2          "STAT2"
// STR_TRIM_OPTS indexed 3 chars each
#define STR_TRIM_OPTS      "\003ExpExFFneMedCrs"
#define STR_TTM            "TTm"
#define STR_FUEL           "Fuel"
#define STR_12_RPM         "\012RPM"
#define STR_LAT_EQ         "Lat="
#define STR_LON_EQ         "Lon="
#define STR_ALT_MAX        "Alt=\011m   Max="
#define STR_SPD_KTS_MAX    "Spd=\011kts Max="
#define STR_11_MPH         "\011mph"

#define STR_SINK_TONES		 "Sink Tones"
#define STR_FRSKY_MOD      "Frsky Mod Done"
#define STR_TEZ_R90				 "TelemetrEZ>r89"

// ersky9x strings
#define STR_ST_CARD_STAT   "SD CARD STAT"
#define STR_4_READY        "\004Klar"
#define STR_NOT            "Ikke"
#define STR_BOOT_REASON    "BOOT REASON"
#define STR_6_WATCHDOG     "\006WATCHDOG"
#define STR_5_UNEXPECTED   "\005UVENTET"
#define STR_6_SHUTDOWN     "\006SHUTDOWN"
#define STR_6_POWER_ON     "\006POWER P�"
// STR_MONTHS indexed 3 chars each
#define STR_MONTHS         "\003XxxJanFebMarAprMaiJunJulAugSepOktNovDes"
#define STR_MENU_REFRESH   "[MENU] for oppdatering"
#define STR_DATE_TIME      "DATO-TID"
#define STR_SEC            "Sek."
#define STR_MIN_SET        "Min.\015Set"
#define STR_HOUR_MENU_LONG "Time\012MENU LONG"
#define STR_DATE           "Dato"
#define STR_MONTH          "M�ned"
#define STR_YEAR_TEMP      "�r\013Temp."
#define STR_YEAR           "�r"
#define STR_BATTERY        "BATTERI"
#define STR_Battery        "Batteri"
#define STR_CURRENT_MAX    "Current\016Max"
#define STR_CPU_TEMP_MAX   "CPU temp.\014C Max\024C"
#define STR_MEMORY_STAT    "MINNE STAT"
#define STR_GENERAL        "Generelt"
#define STR_Model          "Modell"
#define STR_RADIO_SETUP2   "RADIO SETUP2"
#define STR_BRIGHTNESS     "Lysstyrke"
#define STR_CAPACITY_ALARM "Kapasitet Alarm"
#define STR_BT_BAUDRATE    "Bt baudrate"
#define STR_ROTARY_DIVISOR "Rotary Divisor"
#define STR_STICK_LV_GAIN  "Stick LV Gain"
#define STR_STICK_LH_GAIN  "Stick LH Gain"
#define STR_STICK_RV_GAIN  "Stick RV Gain"
#define STR_STICK_RH_GAIN  "Stick RH Gain"

#define STR_DISPLAY					"Display"


