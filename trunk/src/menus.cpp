/*
 * Author - Erez Raviv <erezraviv@gmail.com>
 *
 * Based on th9x -> http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *
 * Needs fixing:
 * TRAINER
 * SETUP01
 * *EXPO
 * *LIMITS
 * *EditMix ONE
 *
 */

#include "er9x.h"
#include "templates.h"

#define IS_THROTTLE(x)  (((2-(g_eeGeneral.stickMode&1)) == x) && (x<4))
#define GET_DR_STATE(x) (!getSwitch(g_model.expoData[x].drSw1,0) ?   \
                          DR_HIGH :                                  \
                          !getSwitch(g_model.expoData[x].drSw2,0)?   \
                          DR_MID : DR_LOW);

#define DO_SQUARE(xx,yy,ww)         \
    lcd_vline(xx-ww/2,yy-ww/2,ww);  \
    lcd_hline(xx-ww/2,yy+ww/2,ww);  \
    lcd_vline(xx+ww/2,yy-ww/2,ww);  \
    lcd_hline(xx-ww/2,yy-ww/2,ww);

#define DO_CROSS(xx,yy,ww)          \
    lcd_vline(xx,yy-ww/2,ww);  \
    lcd_hline(xx-ww/2,yy,ww);  \

#define V_BAR(xx,yy,ll)       \
    lcd_vline(xx-1,yy-ll,ll); \
    lcd_vline(xx  ,yy-ll,ll); \
    lcd_vline(xx+1,yy-ll,ll); \


static int16_t calibratedStick[7];
static uint8_t s_pgOfs;
       uint8_t s_editMode;

int16_t g_chans512[NUM_CHNOUT];

extern bool warble;

//static TrainerData g_trainer;

//sticks
#include "sticks.lbm"
typedef PROGMEM void (*MenuFuncP_PROGMEM)(uint8_t event);

MenuFuncP_PROGMEM APM menuTabModel[] = {
  menuProcModelSelect,
  menuProcModel,
  menuProcExpoAll,
  menuProcMix,
  menuProcLimits,
  menuProcCurve,
  menuProcTemplates
};

MenuFuncP_PROGMEM APM menuTabDiag[] = {
  menuProcSetup0,
  menuProcSetup1,
  menuProcPPMIn,
  menuProcDiagVers,
  menuProcDiagKeys,
  menuProcDiagAna,
  menuProcDiagCalib
};

MenuFuncP lastMenu = menuProcModelSelect;


//#define PARR8(args...) (__extension__({static prog_uint8_t APM __c[] = args;&__c[0];}))
struct MState2
{
  uint8_t m_posVert;
  uint8_t m_posHorz;
  void init(){m_posVert=m_posHorz=0;};
  prog_uint8_t *m_tab;
  static uint8_t event;
  void check_v(uint8_t event,  uint8_t curr,MenuFuncP *menuTab, uint8_t menuTabSize, uint8_t maxrow);
  void check(uint8_t event,  uint8_t curr,MenuFuncP *menuTab, uint8_t menuTabSize, prog_uint8_t*subTab,uint8_t subTabMax,uint8_t maxrow);
};
#define MSTATE_TAB  static prog_uint8_t APM mstate_tab[]
#define MSTATE_CHECK0_VxH(numRows) mstate2.check(event,0,0,0,mstate_tab,DIM(mstate_tab)-1,numRows-1)
#define MSTATE_CHECK0_V(numRows) mstate2.check_v(event,0,0,0,numRows-1)
#define MSTATE_CHECK_VxH(curr,menuTab,numRows) mstate2.check(event,curr,menuTab,DIM(menuTab),mstate_tab,DIM(mstate_tab)-1,numRows-1)
#define MSTATE_CHECK_V(curr,menuTab,numRows) mstate2.check_v(event,curr,menuTab,DIM(menuTab),numRows-1)


void MState2::check_v(uint8_t event,  uint8_t curr,MenuFuncP *menuTab, uint8_t menuTabSize, uint8_t maxrow)
{
  check( event,  curr, menuTab, menuTabSize, 0, 0, maxrow);
}
void MState2::check(uint8_t event,  uint8_t curr,MenuFuncP *menuTab, uint8_t menuTabSize, prog_uint8_t*horTab,uint8_t horTabMax,uint8_t maxrow)
{
  if(menuTab){
    uint8_t attr = m_posVert==0 ? INVERS : 0;
    curr--; //calc from 0, user counts from 1

    if(m_posVert==0){
      switch(event)
      {
        case EVT_KEY_FIRST(KEY_LEFT):
          if(curr>0){
            chainMenu((MenuFuncP)pgm_read_adr(&menuTab[curr-1]));
          }
          break;
        case EVT_KEY_FIRST(KEY_RIGHT):
          if(curr < (menuTabSize-1)){
            chainMenu((MenuFuncP)pgm_read_adr(&menuTab[curr+1]));
          }
          break;
      }
    }
    lcd_putcAtt(128-FW*1,0,menuTabSize+'0',attr);
    lcd_putcAtt(128-FW*2,0,'/',attr);
    lcd_putcAtt(128-FW*3,0,curr+'1',attr);
  }

#define MAXCOL(row) (horTab ? pgm_read_byte(horTab+min( row, horTabMax ))-1 : 0)
#define INC(val,max) if(val<max) {val++;} else {val=0;}
#define DEC(val,max) if(val>0  ) {val--;} else {val=max;}
  uint8_t maxcol = MAXCOL(m_posVert);
  switch(event)
  {
    case EVT_ENTRY:
      //if(m_posVert>maxrow)
        m_posVert=0;
      //init();BLINK_SYNC;
      break;
    case EVT_KEY_LONG(KEY_EXIT):
      s_editMode = false;
      popMenu(true); //return to uppermost, beeps itself
      break;
    case EVT_KEY_BREAK(KEY_EXIT):
      if(s_editMode) {
        s_editMode = false;
        break;
      }
      if(m_posVert==0 || !menuTab) {
        popMenu();  //beeps itself
      } else {
        beepKey();
        init();BLINK_SYNC;
      }
      break;

    case EVT_KEY_REPT(KEY_RIGHT):  //inc
      if(m_posHorz==maxcol) break;
    case EVT_KEY_FIRST(KEY_RIGHT)://inc
      if(!horTab || s_editMode)break;
      INC(m_posHorz,maxcol);
      BLINK_SYNC;
      break;

    case EVT_KEY_REPT(KEY_LEFT):  //dec
      if(m_posHorz==0) break;
    case EVT_KEY_FIRST(KEY_LEFT)://dec
      if(!horTab || s_editMode)break;
      DEC(m_posHorz,maxcol);
      BLINK_SYNC;
      break;

    case EVT_KEY_REPT(KEY_DOWN):  //inc
      if(m_posVert==maxrow) break;
    case EVT_KEY_FIRST(KEY_DOWN): //inc
      if(s_editMode)break;
      INC(m_posVert,maxrow);
      BLINK_SYNC;
      break;

    case EVT_KEY_REPT(KEY_UP):  //dec
      if(m_posVert==0) break;
    case EVT_KEY_FIRST(KEY_UP): //dec
      if(s_editMode)break;
      DEC(m_posVert,maxrow);
      BLINK_SYNC;
      break;

  }
}


//#ifdef SIM
//extern char g_title[80];
//MState2 mstate2;
//#define TITLEP(pstr) lcd_putsAtt(0,0,pstr,INVERS);sprintf(g_title,"%s_%d_%d",pstr,mstate2.m_posVert,mstate2.m_posHorz);
//#else
#define TITLEP(pstr) lcd_putsAtt(0,0,pstr,INVERS)
//#endif
#define TITLE(str)   TITLEP(PSTR(str))


static uint8_t s_curveChan;


void menuProcCurveOne(uint8_t event) {
  static MState2 mstate2;
  uint8_t x = TITLE("CURVE ");
  lcd_outdezAtt(x, 0,s_curveChan+1 ,INVERS);
  int8_t  sub    = mstate2.m_posVert;

  bool    cv9 = s_curveChan >= MAX_CURVE5;
  MSTATE_CHECK0_V((cv9 ? 9 : 5)+1);

  int8_t *crv = cv9 ? g_model.curves9[s_curveChan-MAX_CURVE5] : g_model.curves5[s_curveChan];

    for (uint8_t i = 0; i < 5; i++) {
      uint8_t y = i * FH + 16;
      uint8_t attr = sub == i ? INVERS : 0;
      lcd_outdezAtt(4 * FW, y, crv[i], attr);
    }
  if(cv9)
    for (uint8_t i = 0; i < 4; i++) {
      uint8_t y = i * FH + 16;
      uint8_t attr = sub == i + 5 ? INVERS : 0;
      lcd_outdezAtt(8 * FW, y, crv[i + 5], attr);
    }
  lcd_putsAtt( 2*FW, 7*FH,PSTR("PRESET"),sub == (cv9 ? 9 : 5) ? INVERS : 0);

  static int8_t dfltCrv;
  if(sub<(cv9 ? 9 : 5))  CHECK_INCDEC_H_MODELVAR( event, crv[sub], -100,100);
  else {
    if( checkIncDecGen2(event, &dfltCrv, -4, 4, 0)){
      if(cv9) for (uint8_t i = 0; i < 9; i++) crv[i] = (i-4)*dfltCrv* 100 / 16;
      else    for (uint8_t i = 0; i < 5; i++) crv[i] = (i-2)*dfltCrv* 100 /  8;
      eeDirty(EE_MODEL);
    }
  }

#define WCHART 32
#define X0     (128-WCHART-2)
#define Y0     32
#define RESX    512
#define RESXu   512u
#define RESXul  512ul
#define RESXl   512l
#define RESKul  100ul
#define RESX_PLUS_TRIM (RESX+128)

  for (uint8_t xv = 0; xv < WCHART * 2; xv++) {
    uint16_t yv = intpol(xv * (RESXu / WCHART) - RESXu, s_curveChan) / (RESXu
                                                                      / WCHART);
    lcd_plot(X0 + xv - WCHART, Y0 - yv);
    if ((xv & 3) == 0) {
      lcd_plot(X0 + xv - WCHART, Y0 + 0);
    }
  }
  lcd_vline(X0, Y0 - WCHART, WCHART * 2);
}



void menuProcCurve(uint8_t event) {
  static MState2 mstate2;
  TITLE("CURVE");
  MSTATE_CHECK_V(6,menuTabModel,MAX_CURVE5+MAX_CURVE9+1);
  int8_t  sub    = mstate2.m_posVert - 1;

  if(sub<1) s_pgOfs=0;
  else if((sub-s_pgOfs)>6) s_pgOfs = sub-6;
  else if((sub-s_pgOfs)<0) s_pgOfs = sub;
  if(s_pgOfs<0) s_pgOfs = 0;

  switch (event) {
    case EVT_ENTRY:
      s_pgOfs = 0;
      break;
    case EVT_KEY_FIRST(KEY_MENU):
      if (sub >= 0) {
        s_curveChan = sub;
        pushMenu(menuProcCurveOne);
      }
      break;
  }

  uint8_t y    = 1*FH;
  uint8_t yd   = 1;
  uint8_t m    = 0;
  for (uint8_t i = 0; i < 7; i++) {
    uint8_t k = i + s_pgOfs;
    uint8_t attr = sub == k ? INVERS : 0;
    bool    cv9 = k >= MAX_CURVE5;

    if(cv9 && (yd>6)) break;
    if(yd>7) break;
    if(!m) m = attr;
    lcd_putsAtt(   FW*0, y,PSTR("CV"),attr);
    lcd_outdezAtt( (k<9) ? FW*3 : FW*4-1, y,k+1 ,attr);

    int8_t *crv = cv9 ? g_model.curves9[k-MAX_CURVE5] : g_model.curves5[k];
    for (uint8_t j = 0; j < (5); j++) {
      lcd_outdezAtt( j*(3*FW+3) + 7*FW, y, crv[j], 0);
    }
    y += FH;yd++;
    if(cv9){
      for (uint8_t j = 0; j < 4; j++) {
        lcd_outdezAtt( j*(3*FW+3) + 7*FW, y, crv[j+5], 0);
      }
      y += FH;yd++;
    }
  }

  if(!m) s_pgOfs++;
}



static bool  s_limitCacheOk;
#define LIMITS_DIRTY s_limitCacheOk=false

void setStickCenter() // copy state of 3 primary to subtrim
{
      int16_t zero_chans512[NUM_CHNOUT];
      perOut(zero_chans512,false,true); // do output loop - zero input channels

      for(uint8_t i=0; i<NUM_CHNOUT; i++)
        g_model.limitData[i].offset += zero_chans512[i] - g_chans512[i];

      for(uint8_t i=0; i<4; i++)
        if(!IS_THROTTLE(i)) g_model.trim[i] = 0;// set trims to zero.
      LIMITS_DIRTY;
      beepWarn1();
}

void menuProcLimits(uint8_t event)
{
  static MState2 mstate2;
  static bool swVal[NUM_CHNOUT];
  TITLE("LIMITS");
  MSTATE_TAB = { 4,4};
  MSTATE_CHECK_VxH(5,menuTabModel,NUM_CHNOUT+2);

  uint8_t y = 0;
  uint8_t k = 0;
  int8_t  sub    = mstate2.m_posVert - 1;
  uint8_t subSub = mstate2.m_posHorz + 1;
  if(sub<1) s_pgOfs=0;
  else if((sub-s_pgOfs)>5) s_pgOfs = sub-5;
  else if((sub-s_pgOfs)<0) s_pgOfs = sub;
  if(s_pgOfs<0) s_pgOfs = 0;

  switch(event)
  {
    case EVT_ENTRY:
      s_pgOfs = 0;
      s_editMode = false;
      //mstate2.m_posHorz = -1;
      break;
    case EVT_KEY_FIRST(KEY_MENU):
      if(sub>=0) s_editMode = !s_editMode;
      break;
    case EVT_KEY_LONG(KEY_MENU):
      int16_t v = g_chans512[sub - s_pgOfs];
      LimitData *ld = &g_model.limitData[sub];
      switch (subSub) {
        case 1:
          ld->offset = (ld->revert) ? -v : v;
          LIMITS_DIRTY;
          break;
      }
      break;
  }
  lcd_puts_P( 4*FW, 1*FH,PSTR("subT min  max inv"));
  for(uint8_t i=0; i<6; i++){
    y=(i+2)*FH;
    k=i+s_pgOfs;
    if(k==NUM_CHNOUT) break;
    LimitData *ld = &g_model.limitData[k];
    for(uint8_t j=0; j<=4;j++){
      uint8_t attr = ((sub==k && subSub==j) ? (s_editMode ? BLINK : INVERS) : 0);
      int16_t v = (ld->revert) ? -ld->offset : ld->offset;
      if((g_chans512[k] - v) >  50) swVal[k] = (true==ld->revert);// Switch to raw inputs?  - remove trim!
      if((g_chans512[k] - v) < -50) swVal[k] = (false==ld->revert);
      lcd_putcAtt(12*FW+FW/2, y, (swVal[k] ? 127 : 126),0); //'<' : '>'
      switch(j)
      {
        case 0:
          putsChn(0,y,k+1,(sub==k && subSub==0) ? INVERS : 0);
          break;
        case 1:
          lcd_outdezAtt(  8*FW, y,  ld->offset, attr|PREC1);
          if(attr && s_editMode) {
            if(CHECK_INCDEC_H_MODELVAR( event, ld->offset, -1000,1000))  LIMITS_DIRTY;
          }
          break;
        case 2:
          lcd_outdezAtt(  12*FW, y, (int8_t)(ld->min-100),   attr);
          if(attr && s_editMode) {
            ld->min -=  100;
            if(CHECK_INCDEC_H_MODELVAR( event, ld->min, -125,125))  LIMITS_DIRTY;
            ld->min +=  100;
          }
          break;
        case 3:
          lcd_outdezAtt( 17*FW, y, (int8_t)(ld->max+100),    attr);
          if(attr && s_editMode) {
            ld->max +=  100;
            if(CHECK_INCDEC_H_MODELVAR( event, ld->max, -125,125))  LIMITS_DIRTY;
            ld->max -=  100;
          }
          break;
        case 4:
          lcd_putsnAtt(   18*FW, y, PSTR("---INV")+ld->revert*3,3,attr);
          if(attr && s_editMode) {
            CHECK_INCDEC_H_MODELVAR_BF( event, ld->revert,    0,1);
          }
          break;
      }
    }
  }
  if(k==NUM_CHNOUT){
    //last line available - add the "copy trim menu" line
    uint8_t attr = (sub==NUM_CHNOUT) ? INVERS : 0;
    lcd_putsAtt(  3*FW,y,PSTR("COPY TRIM [MENU]"),attr);
    if(attr && event==EVT_KEY_LONG(KEY_MENU)) {
      s_editMode = false;
      killEvents(event);
      setStickCenter(); //if highlighted and menu pressed - copy trims
    }
  }
}

void applyTemplate(uint8_t idx)
{
  //get mix from mix_Templates
  TemplateData &tmp = mix_Templates[idx];


  for(uint8_t i=0;i<tmp.count;i++) {
    // find next available slot and set j to it
    uint8_t destCh = tmp.md[i].destCh<=4 ? CONVERT_MODE(tmp.md[i].destCh) : tmp.md[i].destCh;
    uint8_t j = 0;
    for(j=0;j<MAX_MIXERS;j++)
      if((g_model.mixData[j].destCh==0) || (g_model.mixData[j].destCh>NUM_CHNOUT) || (g_model.mixData[j].destCh>destCh)) break;

    //move data back
    memmove(&g_model.mixData[j+1],&g_model.mixData[j],
         (MAX_MIXERS-(j+1))*sizeof(MixData) );

    // copy mix data
    MixData md = &g_model.mixData[j];
    memcpy(md,tmp.md[i],sizeof(md));

    //change acording to mode
    if(md.destCh<=4) md.destCh = CONVERT_MODE(md.destCh);
    if(md.srcRaw<=4) md.srcRaw = CONVERT_MODE(md.srcRaw);
  }
  STORE_MODELVARS;
}

void menuProcTemplates(uint8_t event);  //Issue 73
{
  static MState2 mstate2;
  TITLE("TEMPLATES");
  MSTATE_CHECK_V(7,menuTabModel,NUM_TEMPLATES+2);
  int8_t  sub    = mstate2.m_posVert;

  uint8_t y = 0;
  uint8_t k = 0;
  int8_t  sub    = mstate2.m_posVert - 1;
  if(sub<1) s_pgOfs=0;
  else if((sub-s_pgOfs)>6) s_pgOfs = sub-6;
  else if((sub-s_pgOfs)<0) s_pgOfs = sub;
  if(s_pgOfs<0) s_pgOfs = 0;

  switch(event)
  {
    case EVT_ENTRY:
      s_pgOfs = 0;
      break;
    case EVT_KEY_LONG(KEY_MENU):
      killEvents(event);
      //apply mixes or delete
      if(sub==NUM_TEMPLATES)
        memset(g_model.mixData,0,sizeof(g_model.mixData)); //clear all mixes
      else
        applyTemplate(sub-1);
      beepWarn1();
      break;
  }
  for(uint8_t i=0; i<7; i++){
    y=(i+1)*FH;
    k=i+s_pgOfs;
    if(k==NUM_TEMPLATES) break;

    //write mix names here
    lcd_outdezNAtt(3*FW, y, k+1, (sub==k ? INVERS : 0) + LEADING0,2);
    lcd_putsnAtt(  4*FW, y, mix_Templates[k].name,sizeof(mix_Templates[k].name),BSS_NO_INV|(sub==k ? INVERS  : 0));
  }
  if(k==NUM_TEMPLATES){
    uint8_t attr = (sub==NUM_TEMPLATES) ? INVERS : 0;
    lcd_putsAtt(  1*FW,y,PSTR("CLEAR MIXES [MENU]"),attr);
  }
}

static int8_t s_currMixIdx;
static int8_t s_currDestCh;
static bool   s_currMixInsMode;


void deleteMix(uint8_t idx)
{
  memmove(&g_model.mixData[idx],&g_model.mixData[idx+1],
            (MAX_MIXERS-(idx+1))*sizeof(MixData));
  memset(&g_model.mixData[MAX_MIXERS-1],0,sizeof(MixData));
  STORE_MODELVARS;
}

void insertMix(uint8_t idx)
{
  memmove(&g_model.mixData[idx+1],&g_model.mixData[idx],
         (MAX_MIXERS-(idx+1))*sizeof(MixData) );
  memset(&g_model.mixData[idx],0,sizeof(MixData));
  g_model.mixData[idx].destCh      = s_currDestCh; //-s_mixTab[sub];
  g_model.mixData[idx].srcRaw      = s_currDestCh; //1;   //
  g_model.mixData[idx].weight      = 100;
  STORE_MODELVARS;
}

void menuProcMixOne(uint8_t event)
{
  static MState2 mstate2;
  uint8_t x=TITLEP(s_currMixInsMode ? PSTR("INSERT MIX ") : PSTR("EDIT MIX "));
  MixData *md2 = &g_model.mixData[s_currMixIdx];
  //lcd_putsAtt(x*FW, 0,PSTR("Dest->"),0);
  putsChn(x+1*FW,0,md2->destCh,0);
  //MSTATE_TAB = { 1,1,1,1,1,1,1,1,1,1,1};
  //MSTATE_CHECK0_VxH(11);
  MSTATE_CHECK0_V(12);
  int8_t  sub    = mstate2.m_posVert;

  if(sub<1) s_pgOfs=0;
  else if((sub-s_pgOfs)>6) s_pgOfs = sub-6;
  else if((sub-s_pgOfs)<0) s_pgOfs = sub;
  if(s_pgOfs<0) s_pgOfs = 0;

#define CURV_STR "---x>0x<0|x|f>0f<0|f|c1 c2 c3 c4 c5 c6 c7 c8 c9 c10c11c12c13c14c15c16"
  for(uint8_t y=FH; y<8*FH; y+=FH)
  {
    uint8_t i=(y/FH)+s_pgOfs-1;
    uint8_t attr = sub==i ? INVERS : 0;
    switch(i){
      case 0:
        lcd_putsAtt(  2*FW,y,PSTR("Source"),0);
        putsChnRaw(   FW*9,y,md2->srcRaw,attr);
        if(attr) CHECK_INCDEC_H_MODELVAR_BF( event, md2->srcRaw, 1,NUM_XCHNRAW); //!! bitfield
        break;
      case 1:
        lcd_putsAtt(  2*FW,y,PSTR("Weight"),0);
        lcd_outdezAtt(FW*11 + FW/2,y,md2->weight,attr);
        if(attr) CHECK_INCDEC_H_MODELVAR( event, md2->weight, -125,125);
        break;
      case 2:
        lcd_putsAtt(  2*FW,y,PSTR("Offset"),0);
        lcd_outdezAtt(FW*11 + FW/2,y,md2->sOffset,attr);
        if(attr) CHECK_INCDEC_H_MODELVAR( event, md2->sOffset, -125,125);
        break;
      case 3:
        lcd_putsAtt(  2*FW,y,PSTR("Trim"),0);
        lcd_putsnAtt(FW*9,y, PSTR("ON OFF")+3*md2->carryTrim,3,attr);
        if(attr) CHECK_INCDEC_H_MODELVAR_BF( event, md2->carryTrim, 0,1);
        break;
      case 4:
        lcd_putsAtt(  2*FW,y,PSTR("Curves"),0);
        lcd_putsnAtt( FW*9,y,PSTR(CURV_STR)+md2->curve*3,3,attr);
        if(attr) CHECK_INCDEC_H_MODELVAR( event, md2->curve, 0,MAX_CURVE5+MAX_CURVE9+7-1);
        if(attr && md2->curve>=7 && event==EVT_KEY_FIRST(KEY_MENU)){
          s_curveChan = md2->curve-7;
          pushMenu(menuProcCurveOne);
        }
        break;
      case 5:
        lcd_putsAtt(  2*FW,y,PSTR("Switch"),0);
        putsDrSwitches(8*FW,  y,md2->swtch,attr);
        if(attr) CHECK_INCDEC_H_MODELVAR( event, md2->swtch, -MAX_DRSWITCH, MAX_DRSWITCH);
        break;
      case 6:
        lcd_putsAtt(  2*FW,y,PSTR("Multpx"),0);
        lcd_putsnAtt(9*FW, y,PSTR("Add     MultiplyReplace ")+8*md2->mltpx,8,attr);
        if(attr) CHECK_INCDEC_H_MODELVAR_BF( event, md2->mltpx, 0, 2); //!! bitfield
        break;
      case 7:
        lcd_putsAtt(  2*FW,y,PSTR("Delay Down"),0);
        attr = (sub==i) ? INVERS : 0;
        //lcd_putsAtt(9*FW, y, PSTR("Dn"),0);
        lcd_outdezAtt(FW*15,y,md2->delayDown,attr);
        if(attr)  CHECK_INCDEC_H_MODELVAR_BF( event, md2->delayDown, 0,15); //!! bitfield
        break;
      case 8:
        lcd_putsAtt(  2*FW,y,PSTR("Delay Up"),0);
        attr = (sub==i) ? INVERS : 0;
        //lcd_putsAtt(15*FW, y, PSTR("Up"),0);
        lcd_outdezAtt(FW*15,y,md2->delayUp,attr);
        if(attr)  CHECK_INCDEC_H_MODELVAR_BF( event, md2->delayUp, 0,15); //!! bitfield
        break;
      case 9:
        lcd_putsAtt(  2*FW,y,PSTR("Slow  Down"),0);
        attr = (sub==i) ? INVERS : 0;
        //lcd_putsAtt(9*FW, y, PSTR("Dn"),0);
        lcd_outdezAtt(FW*15,y,md2->speedDown,attr);
        if(attr)  CHECK_INCDEC_H_MODELVAR_BF( event, md2->speedDown, 0,15); //!! bitfield
        break;
      case 10:
        lcd_putsAtt(  2*FW,y,PSTR("Slow  Up"),0);
        attr = (sub==i) ? INVERS : 0;
        //lcd_putsAtt(15*FW, y, PSTR("Up"),0);
        lcd_outdezAtt(FW*15,y,md2->speedUp,attr);
        if(attr)  CHECK_INCDEC_H_MODELVAR_BF( event, md2->speedUp, 0,15); //!! bitfield
        break;
      case 11:   lcd_putsAtt(  2*FW,y,PSTR("DELETE MIX [MENU]"),attr);
        if(attr && event==EVT_KEY_LONG(KEY_MENU)){
          killEvents(event);
          deleteMix(s_currMixIdx);
          beepWarn1();
          popMenu();
        }
        break;
    }
  }
}

struct MixTab{
  bool   showCh:1;// show the dest chn
  bool   hasDat:1;// show the data info
  int8_t chId;    //:4  1..NUM_XCHNOUT  dst chn id
  int8_t selCh;   //:5  1..MAX_MIXERS+NUM_XCHNOUT sel sequence
  int8_t selDat;  //:5  1..MAX_MIXERS+NUM_XCHNOUT sel sequence
  int8_t insIdx;  //:5  0..MAX_MIXERS-1        insert index into mix data tab
  int8_t editIdx; //:5  0..MAX_MIXERS-1        edit   index into mix data tab
} s_mixTab[MAX_MIXERS+NUM_XCHNOUT+1];
int8_t s_mixMaxSel;

void genMixTab()
{
  uint8_t maxDst  = 0;
  uint8_t mtIdx   = 0;
  uint8_t sel     = 1;
  memset(s_mixTab,0,sizeof(s_mixTab));

  MixData *md=g_model.mixData;

  for(uint8_t i=0; i<MAX_MIXERS; i++)
  {
    uint8_t destCh = md[i].destCh;
    if(destCh==0) destCh=NUM_XCHNOUT;
    if(destCh > maxDst){
      while(destCh > maxDst){ //ch-loop, hole alle channels auf
        maxDst++;
        s_mixTab[mtIdx].chId  = maxDst; //mark channel header
        s_mixTab[mtIdx].showCh = true;
        s_mixTab[mtIdx].selCh = sel++; //vorab vergeben, falls keine dat
        s_mixTab[mtIdx].insIdx= i;     //
        mtIdx++;
      }
      mtIdx--; //folding: letztes ch bekommt zusaetzlich dat
      s_mixMaxSel =sel;
      sel--; //letzte zeile hat dat, falls nicht ist selCh schon belegt
    }
    if(md[i].destCh==0) break; //letzter eintrag in mix data tab
    s_mixTab[mtIdx].chId    = destCh; //mark channel header
    s_mixTab[mtIdx].editIdx = i;
    s_mixTab[mtIdx].hasDat  = true;
    s_mixTab[mtIdx].selDat  = sel++;
    if(md[i].destCh == md[i+1].destCh){
      s_mixTab[mtIdx].selCh  = 0; //ueberschreibt letzte Zeile von ch-loop
      s_mixTab[mtIdx].insIdx = 0; //
    }
    else{
      s_mixTab[mtIdx].selCh  = sel++;
      s_mixTab[mtIdx].insIdx = i+1; //
    }
    s_mixMaxSel =sel;
    mtIdx++;
  }
}

void moveMix(uint8 idx, uint8_t dir) //true=up false=down - Issue 49
{
  if(idx>MAX_MIXERS || (idx==0 && !dir) || (idx==MAX_MIXERS && dir)) return;

  uint8_t tdx = dir ? idx+1 : idx-1;
  MixData &src=g_model.mixData[idx];
  MixData &tgt=g_model.mixData[tdx];

  if((src.destCh==0) || (src.destCh>NUM_CHNOUT) || (tgt.destCh==0) || (tgt.destCh>NUM_CHNOUT)) return;

  if(tgt.destCh!=src.destCh) { //if not the same scr channel then make the same and exit
    tgt.destCh=src.destCh;
    return;
  }

  //flip between idx and tgt
  MixData temp;
  memcpy(temp,src,sizeof(MixData));
  memcpy(src,tgt,sizeof(MixData));
  memcpy(tgt,temp,sizeof(MixData));

}

void menuProcMix(uint8_t event)
{
  static MState2 mstate2;
  TITLE("MIXER");
  MSTATE_CHECK_V(4,menuTabModel,s_mixMaxSel);
  int8_t  sub    = mstate2.m_posVert;

  MixData *md=g_model.mixData;
  switch(event)
  {
    case EVT_ENTRY:
      s_pgOfs=0;
      k = 0;
      s_editMode=false;
    case EVT_ENTRY_UP:
      genMixTab();
      break;
    case EVT_KEY_FIRST(KEY_MENU):
      if(sub>=1) s_editMode = !s_editMode;
      if(s_currMixInsMode) s_editMode = false; //cant have edit mode if mix not selected
      break;
    case EVT_KEY_FIRST(KEY_DOWN):
    case EVT_KEY_FIRST(KEY_UP):
      if(s_editMode) {
        killEvents(event);
        moveMix(s_currMixIdx,event==EVT_KEY_FIRST(KEY_UP)); // true=up false=down
      }
      break;
    case EVT_KEY_FIRST(KEY_EXIT):
      s_editMode = false;
      break;
    case EVT_KEY_LONG(KEY_MENU):
      if(sub<1) break;
      if(s_currMixInsMode) insertMix(s_currMixIdx);
      pushMenu(menuProcMixOne);
      break;
  }

  int8_t markedIdx=-1;
  uint8_t i;
  int8_t minSel=99;
  int8_t maxSel=-1;
  lcd_puts_P( 6*FW, 1*FH,PSTR("wt src  sw crv"));
  for(i=0; i<6; i++){
    uint8_t y = i * FH + 2*FH;
    uint8_t k = i + s_pgOfs;
    if(!s_mixTab[k].showCh && !s_mixTab[k].hasDat ) break;

    if(s_mixTab[k].showCh){
      putsChn(0,y,s_mixTab[k].chId,0); // show CHx
    }
    if(sub>0 && sub==s_mixTab[k].selCh) { //handle CHx is selected (other line)
      //if(BLINK_ON_PHASE)
      lcd_hline(0,y+7,FW*4);
      s_currMixIdx     = s_mixTab[k].insIdx;
      s_currDestCh     = s_mixTab[k].chId;
      s_currMixInsMode = true;
      markedIdx        = i;
    }
    if(s_mixTab[k].hasDat){ //show data
      MixData *md2=&md[s_mixTab[k].editIdx];
      uint8_t attr = sub==s_mixTab[k].selDat ? INVERS : 0;
      if(!s_mixTab[k].showCh) lcd_putsnAtt(   3*FW, y, PSTR("+*R")+1*md2->mltpx,1,0);
      lcd_outdezAtt(  7*FW+FW/2, y, md2->weight,attr);
      lcd_putcAtt(    7*FW+FW/2, y, '%',s_editMode ? attr : 0);
      putsChnRaw(     9*FW, y, md2->srcRaw,0);
      if(md2->swtch)putsDrSwitches( 13*FW, y, md2->swtch,s_editMode ? attr : 0);
      if(md2->curve)lcd_putsnAtt(   17*FW, y, PSTR(CURV_STR)+md2->curve*3,3,s_editMode ? attr : 0);

      bool bs = (md2->speedDown || md2->speedUp);
      bool bd = (md2->delayUp || md2->delayDown);
      char cs = (bs && bd) ? '*' : (bs ? 'S' : 'D');
      if(bs || bd) lcd_putcAtt(20*FW+1, y, cs,s_editMode ? attr : 0);

      if(attr == INVERS){ //handle dat is selected
        CHECK_INCDEC_H_MODELVAR( event, md2->weight, -125,125);
        s_currMixIdx     = s_mixTab[k].editIdx;
        s_currDestCh     = s_mixTab[k].chId;
        s_currMixInsMode = false;
        markedIdx        = i;

      }
    }
    //welche sub-indize liegen im sichtbaren bereich?
    if(s_mixTab[k].selCh){
      minSel = min(minSel,s_mixTab[k].selCh);
      maxSel = max(maxSel,s_mixTab[k].selCh);
    }
    if(s_mixTab[k].selDat){
      minSel = min(minSel,s_mixTab[k].selDat);
      maxSel = max(maxSel,s_mixTab[k].selDat);
    }

  } //for 7
  if( sub!=0 &&  markedIdx==-1) { //versuche die Marke zu finden
    if(sub < minSel) s_pgOfs = max(0,s_pgOfs-1);
    if(sub > maxSel) s_pgOfs++;
  }
  else if(markedIdx<=1)              s_pgOfs = max(0,s_pgOfs-1);
  else if(markedIdx>=5 && i>=7)      s_pgOfs++;

}



uint16_t expou(uint16_t x, uint16_t k)
{
  // k*x*x*x + (1-k)*x
  return ((unsigned long)x*x*x/0x10000*k/(RESXul*RESXul/0x10000) + (RESKul-k)*x+RESKul/2)/RESKul;
}
// expo-funktion:
// ---------------
// kmplot
// f(x,k)=exp(ln(x)*k/10) ;P[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]
// f(x,k)=x*x*x*k/10 + x*(1-k/10) ;P[0,1,2,3,4,5,6,7,8,9,10]
// f(x,k)=x*x*k/10 + x*(1-k/10) ;P[0,1,2,3,4,5,6,7,8,9,10]
// f(x,k)=1+(x-1)*(x-1)*(x-1)*k/10 + (x-1)*(1-k/10) ;P[0,1,2,3,4,5,6,7,8,9,10]

int16_t expo(int16_t x, int16_t k)
{
  if(k == 0) return x;
  int16_t   y;
  bool    neg =  x < 0;
  if(neg)   x = -x;
  if(k<0){
    y = RESXu-expou(RESXu-x,-k);
  }else{
    y = expou(x,k);
  }
  return neg? -y:y;
}


#ifdef EXTENDED_EXPO
/// expo with y-offset
class Expo
{
  uint16_t   c;
  int16_t    d,drx;
public:
  void     init(uint8_t k, int8_t yo);
  static int16_t  expou(uint16_t x,uint16_t c, int16_t d);
  int16_t  expo(int16_t x);
};
void    Expo::init(uint8_t k, int8_t yo)
{
  c = (uint16_t) k  * 256 / 100;
  d = (int16_t)  yo * 256 / 100;
  drx = d * ((uint16_t)RESXu/256);
}
int16_t Expo::expou(uint16_t x,uint16_t c, int16_t d)
{
  uint16_t a = 256 - c - d;
  if( (int16_t)a < 0 ) a = 0;
  // a x^3 + c x + d
  //                         9  18  27        11  20   18
  uint32_t res =  ((uint32_t)x * x * x / 0x10000 * a / (RESXul*RESXul/0x10000) +
                   (uint32_t)x                   * c
  ) / 256;
  return (int16_t)res;
}
int16_t  Expo::expo(int16_t x)
{
  if(c==256 && d==0) return x;
  if(x>=0) return expou(x,c,d) + drx;
  return -expou(-x,c,-d) + drx;
}
#endif

static uint8_t s_expoChan;

void editExpoVals(uint8_t event,uint8_t stopBlink,uint8_t editMode, uint8_t edit,uint8_t x, uint8_t y, uint8_t chn, uint8_t which, uint8_t exWt, uint8_t stkRL)
{
  uint8_t  invBlk = edit ? (editMode ? BLINK : INVERS) : 0;
  if(edit && stopBlink) invBlk = INVERS;

  if(which==DR_DRSW1) {
    putsDrSwitches(x,y,g_model.expoData[chn].drSw1,invBlk);
    if(edit && editMode) CHECK_INCDEC_H_MODELVAR(event,g_model.expoData[chn].drSw1,-MAX_DRSWITCH,MAX_DRSWITCH);
  }
  else if(which==DR_DRSW2) {
    putsDrSwitches(x,y,g_model.expoData[chn].drSw2,invBlk);
    if(edit && editMode) CHECK_INCDEC_H_MODELVAR(event,g_model.expoData[chn].drSw2,-MAX_DRSWITCH,MAX_DRSWITCH);
  }
  else
    if(exWt==DR_EXPO){
      lcd_outdezAtt(x, y, g_model.expoData[chn].expo[which][exWt][stkRL], invBlk);
      if(edit && editMode) CHECK_INCDEC_H_MODELVAR(event,g_model.expoData[chn].expo[which][exWt][stkRL],-100, 100);
    }
    else {
      lcd_outdezAtt(x, y, g_model.expoData[chn].expo[which][exWt][stkRL]+100, invBlk);
      if(edit && editMode) CHECK_INCDEC_H_MODELVAR(event,g_model.expoData[chn].expo[which][exWt][stkRL],-100, 0);
    }
}

void menuProcExpoOne(uint8_t event)
{
  static MState2 mstate2;
  static uint8_t stkVal;
  uint8_t x=TITLE("EXPO/DR ");
  putsChnRaw(x,0,s_expoChan+1,0);
  MSTATE_CHECK0_V(4);
  int8_t  sub    = mstate2.m_posVert;

   switch(event)
  {
    case EVT_ENTRY:
      s_editMode = false;
      break;
    case EVT_KEY_FIRST(KEY_MENU):
      s_editMode = false;
      break;
  }
  uint8_t expoDrOn = GET_DR_STATE(s_expoChan);
  uint8_t  y = 16;

  if(calibratedStick[s_expoChan]> 25) stkVal = DR_RIGHT;
  if(calibratedStick[s_expoChan]<-25) stkVal = DR_LEFT;
  if(IS_THROTTLE(s_expoChan) && g_model.thrExpo) stkVal = DR_RIGHT;

  lcd_puts_P(0,y,PSTR("Expo"));
  editExpoVals(event,true,true,sub==0,9*FW, y,s_expoChan, expoDrOn ,DR_EXPO,stkVal);
  y+=FH;
  lcd_puts_P(0,y,PSTR("Weight"));
  editExpoVals(event,true,true,sub==1,9*FW, y,s_expoChan, expoDrOn ,DR_WEIGHT,stkVal);
  y+=FH;
  lcd_puts_P(0,y,PSTR("DrSw1"));
  editExpoVals(event,true,true,sub==2,5*FW, y,s_expoChan, DR_DRSW1 , 0,0);
  y+=FH;
  lcd_puts_P(0,y,PSTR("DrSw2"));
  editExpoVals(event,true,true,sub==3,5*FW, y,s_expoChan, DR_DRSW2 , 0,0);
  y+=FH;
  switch (expoDrOn) {
    case DR_MID:
      lcd_puts_P(0,y,PSTR("DR Mid"));
      break;
    case DR_LOW:
      lcd_puts_P(0,y,PSTR("DR Low"));
      break;
    default: // DR_HIGH:
      lcd_puts_P(0,y,PSTR("DR High"));
      break;
  }
  y+=FH;


  int8_t   kViewR  = g_model.expoData[s_expoChan].expo[expoDrOn][DR_EXPO][DR_RIGHT];  //NormR;
  int8_t   kViewL  = g_model.expoData[s_expoChan].expo[expoDrOn][DR_EXPO][DR_LEFT];  //NormL;
  int8_t   wViewR  = g_model.expoData[s_expoChan].expo[expoDrOn][DR_WEIGHT][DR_RIGHT]+100;  //NormWeightR+100;
  int8_t   wViewL  = g_model.expoData[s_expoChan].expo[expoDrOn][DR_WEIGHT][DR_LEFT]+100;  //NormWeightL+100;


#define WCHART 32
#define X0     (128-WCHART-2)
#define Y0     32

  if (IS_THROTTLE(s_expoChan) && g_model.thrExpo)
       for(uint8_t xv=0;xv<WCHART*2;xv++)
    {
      uint16_t yv=2*expo(xv*(RESXu/WCHART)/2,kViewR) / (RESXu/WCHART);
      yv = (yv * wViewR)/100;
      lcd_plot(X0+xv-WCHART, 2*Y0-yv);
      if((xv&3) == 0){
        lcd_plot(X0+xv-WCHART, 2*Y0-1);
        lcd_plot(X0-WCHART   , Y0+xv/2);
      }
    }
  else
    for(uint8_t xv=0;xv<WCHART;xv++)
    {
      uint16_t yv=expo(xv*(RESXu/WCHART),kViewR) / (RESXu/WCHART);
      yv = (yv * wViewR)/100;
      lcd_plot(X0+xv, Y0-yv);
      if((xv&3) == 0){
        lcd_plot(X0+xv, Y0+0);
        lcd_plot(X0  , Y0+xv);
      }

      yv=expo(xv*(RESXu/WCHART),kViewL) / (RESXu/WCHART);
      yv = (yv * wViewL)/100;
      lcd_plot(X0-xv, Y0+yv);
      if((xv&3) == 0){
        lcd_plot(X0-xv, Y0+0);
        lcd_plot(X0  , Y0-xv);
      }
    }

  int16_t x512  = calibratedStick[s_expoChan];
  lcd_vline(X0+x512/(RESXu/WCHART), Y0-WCHART,WCHART*2);

  int16_t y512 = 0;
  if (IS_THROTTLE(s_expoChan) && g_model.thrExpo) {
    y512  = 2*expo((x512+RESX)/2,kViewR);
    y512 = y512 * (wViewR / 4)/(100 / 4);
    lcd_hline(X0-WCHART, 2*Y0-y512/(RESXu/WCHART),WCHART*2);
    y512 /= 2;
  }
  else {
    y512  = expo(x512,(x512>0 ? kViewR : kViewL));
    y512 = y512 * ((x512>0 ? wViewR : wViewL) / 4)/(100 / 4);
    lcd_hline(X0-WCHART, Y0-y512/(RESXu/WCHART),WCHART*2);
  }

  lcd_outdezAtt( 19*FW, 6*FH,x512*25/((signed) RESXu/4), 0 );
  lcd_outdezAtt( 14*FW, 1*FH,y512*25/((signed) RESXu/4), 0 );
  //dy/dx
  int16_t dy  = x512>0 ? y512-expo(x512-20,(x512>0 ? kViewR : kViewL)) : expo(x512+20,(x512>0 ? kViewR : kViewL))-y512;
  lcd_outdezNAtt(14*FW, 2*FH,   dy*(100/20), LEADING0|PREC2,3);
}

void menuProcExpoAll(uint8_t event)
{
  static MState2 mstate2;
  static uint8_t stkVal[4];
  TITLE("EXPO/DR");
  MSTATE_TAB = {5,4};
  MSTATE_CHECK_VxH(3,menuTabModel,4+1);
  int8_t  sub    = mstate2.m_posVert - 1;
  int8_t  subHor = mstate2.m_posHorz;

  switch(event)
  {
    case EVT_ENTRY:
      s_editMode = false;
      //mstate2.m_posHorz = -1;
      break;
    case EVT_KEY_FIRST(KEY_MENU):
      if(sub>=0) s_editMode = !s_editMode;
      break;
    case EVT_KEY_LONG(KEY_MENU):
      if(sub>=0){
        s_expoChan = sub;
        pushMenu(menuProcExpoOne);
      }
      break;
  }

  lcd_puts_P( 4*FW-FW/2, 1*FH,PSTR("exp  %  sw1 sw2"));
  for(uint8_t i=0; i<4; i++)
  {
    uint8_t expoDrOn = GET_DR_STATE(i);
    uint8_t valsEqual = (g_model.expoData[i].expo[expoDrOn][DR_WEIGHT][DR_LEFT]==g_model.expoData[i].expo[expoDrOn][DR_WEIGHT][DR_RIGHT]) &&
                        (g_model.expoData[i].expo[expoDrOn][DR_EXPO][DR_LEFT]==g_model.expoData[i].expo[expoDrOn][DR_EXPO][DR_RIGHT]);
    uint8_t stickCentred = (abs(calibratedStick[i])<=25) && valsEqual;
    if(calibratedStick[i]> 25) stkVal[i] = DR_RIGHT;
    if(calibratedStick[i]<-25) stkVal[i] = DR_LEFT;
    if(IS_THROTTLE(i) && g_model.thrExpo) {
      stkVal[i] = DR_RIGHT;
      stickCentred = true;
    }

    uint8_t y=(i+2)*FH;
    putsChnRaw( 0, y,i+1,0);
    uint8_t stkOp = (stkVal[i] == DR_RIGHT) ? DR_LEFT : DR_RIGHT;

    editExpoVals(event,false,s_editMode,sub==i && subHor==0, 7*FW-FW/2, y,i,expoDrOn,DR_EXPO,stkVal[i]);
    if(sub==i && subHor==0 && s_editMode && stickCentred)
      CHECK_INCDEC_H_MODELVAR(event,g_model.expoData[i].expo[expoDrOn][DR_EXPO][stkOp],-100, 100);

    editExpoVals(event,false,s_editMode,sub==i && subHor==1, 9*FW+FW/2, y,i,expoDrOn,DR_WEIGHT,stkVal[i]);
    if(sub==i && subHor==1 && s_editMode && stickCentred)
      CHECK_INCDEC_H_MODELVAR(event,g_model.expoData[i].expo[expoDrOn][DR_WEIGHT][stkOp],-100, 0);

    editExpoVals(event,false,s_editMode,sub==i && subHor==2,10*FW+FW/2, y,i,DR_DRSW1,0,0);
    editExpoVals(event,false,s_editMode,sub==i && subHor==3,14*FW+FW/2, y,i,DR_DRSW2,0,0);
    lcd_putcAtt(9*FW+FW/2 + ((!stkVal[i] && !stickCentred) ? 2 : 1 ), y, stickCentred ? '-' : (stkVal[i] ? 127 : 126),0);//'|' : (stkVal[i] ? '<' : '>'),0);
    switch (expoDrOn) {
    case DR_MID:
      lcd_putcAtt(19*FW+FW/2,y,'M',0);
      break;
    case DR_LOW:
      lcd_putcAtt(19*FW+FW/2,y,'L',0);
      break;
    default: // DR_HIGH:
      lcd_putcAtt(19*FW+FW/2,y,'H',0);
      break;
    }

    /*
    if(g_model.expoData[i].drSw){
      editExpoVals(event,3,sub==i && subHor==3,17*FW, y,i,stkVal[i]);
      editExpoVals(event,4,sub==i && subHor==4,21*FW, y,i,stkVal[i]);
    }else{
      if(sub==i && subHor>=3) mstate2.m_posHorz=2;
    }
    */
  }
}
const prog_char APM s_charTab[]=" ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-.";
#define NUMCHARS (sizeof(s_charTab)-1)

uint8_t char2idx(char c)
{
  for(int8_t ret=0;;ret++)
  {
    char cc= pgm_read_byte(s_charTab+ret);
    if(cc==c) return ret;
    if(cc==0) return 0;
  }
}
char idx2char(uint8_t idx)
{
  if(idx < NUMCHARS) return pgm_read_byte(s_charTab+idx);
  return ' ';
}

void menuProcModel(uint8_t event)
{
  static MState2 mstate2;
  uint8_t x=TITLE("SETUP ");
  lcd_outdezNAtt(x+2*FW,0,g_eeGeneral.currModel+1,INVERS+LEADING0,2);
  MSTATE_TAB = { 1,sizeof(g_model.name),2,1,1,1,1,1,3,1,1};
  MSTATE_CHECK_VxH(2,menuTabModel,10);
  int8_t  sub    = mstate2.m_posVert;
  uint8_t subSub = mstate2.m_posHorz + 1;

  if(sub<1) s_pgOfs=0;
  else if((sub-s_pgOfs)>7) s_pgOfs = sub-7;
  else if((sub-s_pgOfs)<1) s_pgOfs = sub-1;
  if(s_pgOfs<0) s_pgOfs = 0;

  uint8_t y = 1*FH;

  switch(event){
    case EVT_ENTRY:
      s_editMode = false;
      break;
    case EVT_KEY_FIRST(KEY_MENU):
      s_editMode = (sub==1 || sub==2 || sub==7) ? !s_editMode : false;
      break;
    case EVT_KEY_REPT(KEY_LEFT):
    case EVT_KEY_FIRST(KEY_LEFT):
      if(sub==1 && subSub>1 && s_editMode) mstate2.m_posHorz--;
      break;
    case EVT_KEY_REPT(KEY_RIGHT):
    case EVT_KEY_FIRST(KEY_RIGHT):
      if(sub==1 && subSub<sizeof(g_model.name) && s_editMode) mstate2.m_posHorz++;
      break;
    case EVT_KEY_REPT(KEY_UP):
    case EVT_KEY_FIRST(KEY_UP):
    case EVT_KEY_REPT(KEY_DOWN):
    case EVT_KEY_FIRST(KEY_DOWN):
      if (!s_editMode) mstate2.m_posHorz = 0;
      break;
  }

  uint8_t subN = 1;
  if(s_pgOfs<subN) {
    lcd_putsAtt(    0,    y, PSTR("Name"),0);
    lcd_putsnAtt(10*FW,   y, g_model.name ,sizeof(g_model.name),BSS_NO_INV | (sub==1 ? (s_editMode ? 0 : INVERS) : 0));
    if(sub==1 && s_editMode){
        char v = char2idx(g_model.name[subSub-1]);
        if(event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_DOWN) || event==EVT_KEY_REPT(KEY_UP))
           CHECK_INCDEC_H_MODELVAR_BF( event,v ,0,NUMCHARS-1);
        v = idx2char(v);
        g_model.name[subSub-1]=v;
        lcd_putcAtt((10+subSub-1)*FW, 1*FH, v,INVERS);
    }
    if((y+=FH)>8*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
    lcd_putsAtt(    0,    y, PSTR("Timer"),0);
    putsTime(       9*FW, y, g_model.tmrVal,(sub==subN && subSub==1 ? (s_editMode ? BLINK : INVERS):0),(sub==subN && subSub==2 ? (s_editMode ? BLINK : INVERS):0) );

    if(sub==subN && s_editMode)
      switch (subSub) {
       case 1:
          {
          int8_t min=g_model.tmrVal/60;
          CHECK_INCDEC_H_MODELVAR_BF( event,min ,0,59);
          g_model.tmrVal = g_model.tmrVal%60 + min*60;
         break;
          }
        case 2:
          {
          int8_t sec=g_model.tmrVal%60;
          sec -= checkIncDec_hm( event,sec+2 ,1,62)-2;
          g_model.tmrVal -= sec ;
          if((int16_t)g_model.tmrVal < 0) g_model.tmrVal=0;
          break;
          }
      }
    if((y+=FH)>8*FH) return;
  }subN++;

  if(s_pgOfs<subN) { //timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw, chx(value > or < than tmrChVal), ch%
    lcd_putsAtt(    0,    y, PSTR("Trigger"),0);
    uint8_t attr = (sub==subN ?  INVERS : 0);
    int8_t tm = g_model.tmrMode;
    if(abs(tm)<10)
      lcd_putsnAtt(  16*FW, y, PSTR("OFFABSTHSTH%")+3*tm,3,attr);
    else if(abs(g_model.tmrMode)<50)
      putsDrSwitches(15*FW,y,tm>0 ? tm-10 : tm+10,attr);//normal on-off
    else {
      putsDrSwitches(15*FW,y,tm>0 ? tm-50 : tm+50,attr);//momentary on-off
      lcd_putcAtt(19*FW,  y,'m',attr);
    }

    if(sub==subN) {
          CHECK_INCDEC_H_MODELVAR( event,g_model.tmrMode ,-50-MAX_DRSWITCH+1,50+MAX_DRSWITCH-1);
          switch (g_model.tmrMode) {
            case (4):
              g_model.tmrMode = 11;
              break;
            case (11+MAX_DRSWITCH-1):
              g_model.tmrMode = 51;
              break;
            case (50):
              g_model.tmrMode = 10+MAX_DRSWITCH-1;
              break;
            case (10):
              g_model.tmrMode = 3;
              break;
            case (-1):
              g_model.tmrMode = -11;
              break;
            case (-11-MAX_DRSWITCH+1):
              g_model.tmrMode = -51;
              break;
            case (-50):
              g_model.tmrMode = -10-MAX_DRSWITCH+1;
              break;
            case (-10):
              g_model.tmrMode = 0;
              break;
          }
    }
    if((y+=FH)>8*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
    lcd_putsAtt(    0,    y, PSTR("T-Trim"),0);
    lcd_putsnAtt(  10*FW, y, PSTR("OFFON ")+3*g_model.thrTrim,3,(sub==subN ? INVERS:0));
    if(sub==subN) CHECK_INCDEC_H_MODELVAR_BF(event,g_model.thrTrim,0,1);
    if((y+=FH)>8*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
    lcd_putsAtt(    0,    y, PSTR("T-Expo"),0);
    lcd_putsnAtt(  10*FW, y, PSTR("OFFON ")+3*g_model.thrExpo,3,(sub==subN ? INVERS:0));
    if(sub==subN) CHECK_INCDEC_H_MODELVAR_BF(event,g_model.thrExpo,0,1);
    if((y+=FH)>8*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
    lcd_putsAtt(    0,    y, PSTR("Trim Inc"),0);
    lcd_putsnAtt(  10*FW, y, PSTR("Exp   ExFineFine  MediumCoarse")+6*g_model.trimInc,6,(sub==subN ? INVERS:0));
    if(sub==subN) CHECK_INCDEC_H_MODELVAR(event,g_model.trimInc,0,4);
    if((y+=FH)>8*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
    lcd_putsAtt(    0,    y, PSTR("Trim Sw"),0);
    putsDrSwitches(9*FW,y,g_model.trimSw,sub==subN ? INVERS:0);
    if(sub==subN) CHECK_INCDEC_H_MODELVAR(event,g_model.trimSw,-MAX_DRSWITCH, MAX_DRSWITCH);
    if((y+=FH)>8*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
    lcd_putsAtt(    0,    y, PSTR("Proto"),0);//sub==2 ? INVERS:0);
    lcd_putsnAtt(  6*FW, y, PSTR(PROT_STR)+PROT_STR_LEN*g_model.protocol,PROT_STR_LEN,
                  (sub==subN && subSub==1 ? (s_editMode ? BLINK : INVERS):0));
    if(!g_model.protocol) {
      lcd_putsnAtt(  10*FW, y, PSTR("4CH 6CH 8CH 10CH12CH14CH16CH")+4*(g_model.ppmNCH+2),4,(sub==subN && subSub==2  ? (s_editMode ? BLINK : INVERS):0));
      lcd_putsAtt(    17*FW,    y, PSTR("uSec"),0);
      lcd_outdezAtt(  17*FW, y,  (g_model.ppmDelay*50)+300, (sub==subN && subSub==3 ? (s_editMode ? BLINK : INVERS):0));
    }
    if(sub==subN && s_editMode)
      switch (subSub){
        case 1:
            CHECK_INCDEC_H_MODELVAR(event,g_model.protocol,0,PROT_MAX);
            break;
        case 2:
            CHECK_INCDEC_H_MODELVAR(event,g_model.ppmNCH,-2,4);
            break;
        case 3:
            CHECK_INCDEC_H_MODELVAR(event,g_model.ppmDelay,-4,10);
            break;
      }
    if((y+=FH)>8*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
    lcd_putsAtt(0*FW, y, PSTR("TRIM->subTRIM  [MENU]"),sub==subN ? INVERS:0);
    if(sub==subN && event==EVT_KEY_LONG(KEY_MENU)){
        s_editMode = false;
        killEvents(event);
        setStickCenter();
    }
    if((y+=FH)>8*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
    lcd_putsAtt(0*FW, y, PSTR("DELETE MODEL   [MENU]"),sub==subN?INVERS:0);
    if(sub==subN && event==EVT_KEY_LONG(KEY_MENU)){
        s_editMode = false;
        killEvents(event);
        //if(question(PSTR("Delete Model?"))){
          EFile::rm(FILE_MODEL(g_eeGeneral.currModel)); //delete file
          eeLoadModel(g_eeGeneral.currModel); //load default values
          chainMenu(menuProcModelSelect);
        //
    }
    if((y+=FH)>8*FH) return;
  }subN++;
}


void menuProcModelSelect(uint8_t event)
{
  static MState2 mstate2;
  TITLE("MODELSEL");
  lcd_puts_P(     10*FW, 0, PSTR("free"));
  lcd_outdezAtt(  18*FW, 0, EeFsGetFree(),0);
  lcd_putsAtt(128-FW*3,0,PSTR("1/7"),INVERS);
  int8_t subOld  = mstate2.m_posVert;
  MSTATE_CHECK0_V(MAX_MODELS);
  int8_t  sub    = mstate2.m_posVert;
  static uint8_t sel_editMode;
  switch(event)
  {
    //case  EVT_KEY_FIRST(KEY_MENU):
    case  EVT_KEY_FIRST(KEY_EXIT):
      if(sel_editMode){
        sel_editMode = false;
        beepKey();
        killEvents(event);
        break;
      }
      //fallthrough
    case  EVT_KEY_FIRST(KEY_RIGHT):
      if(g_eeGeneral.currModel != mstate2.m_posVert)
      {
        eeLoadModel(g_eeGeneral.currModel = mstate2.m_posVert);
        eeDirty(EE_GENERAL);
        LIMITS_DIRTY;
        beepKey();
      }
      //case EXIT handled in checkExit
      if(event==EVT_KEY_FIRST(KEY_RIGHT))  chainMenu(menuProcModel);
      if(event==EVT_KEY_FIRST(KEY_EXIT))  pushMenu(menuProcModelSelect);
      break;
    case  EVT_KEY_FIRST(KEY_MENU):
      sel_editMode = true;
      beepKey();
      break;
    case  EVT_KEY_LONG(KEY_MENU):
      if(sel_editMode){
        if(eeDuplicateModel(sub)) {
          beepKey();
          sel_editMode = false;
        }
        else beepWarn();
      }
      break;

    case EVT_ENTRY:
      sel_editMode = false;

      mstate2.m_posVert = g_eeGeneral.currModel;
      eeCheck(true); //force writing of current model data before this is changed
      break;
  }
  if(sel_editMode && subOld!=sub){
    EFile::swap(FILE_MODEL(subOld),FILE_MODEL(sub));
  }

  if(sub-s_pgOfs < 1)        s_pgOfs = max(0,sub-1);
  else if(sub-s_pgOfs >4 )  s_pgOfs = min(MAX_MODELS-6,sub-4);
  for(uint8_t i=0; i<6; i++){
    uint8_t y=(i+2)*FH;
    uint8_t k=i+s_pgOfs;
    lcd_outdezNAtt(  3*FW, y, k+1, ((sub==k) ? INVERS : 0) + LEADING0,2);
    static char buf[sizeof(g_model.name)+5];
    if(k==g_eeGeneral.currModel) lcd_putcAtt(1,  y,'*',0);
    eeLoadModelName(k,buf,sizeof(buf));
    lcd_putsnAtt(  4*FW, y, buf,sizeof(buf),BSS_NO_INV|((sub==k) ? (sel_editMode ? INVERS : 0 ) : 0));
  }

}



void menuProcDiagCalib(uint8_t event)
{
  static MState2 mstate2;
  TITLE("CALIB");
  MSTATE_CHECK_V(7,menuTabDiag,4);
  int8_t  sub    = mstate2.m_posVert ;
  static int16_t midVals[7];
  static int16_t loVals[7];
  static int16_t hiVals[7];

  for(uint8_t i=0; i<7; i++) { //get low and high vals for sticks and trims
    int16_t vt = anaIn(i);
    loVals[i] = min(vt,loVals[i]);
    hiVals[i] = max(vt,hiVals[i]);
    if(i>=4) midVals[i] = (loVals[i] + hiVals[i])/2;
  }

  switch(event)
  {
    case EVT_ENTRY:
      for(uint8_t i=0; i<7; i++) loVals[i] = 15000;
      break;
    case EVT_KEY_BREAK(KEY_DOWN): // !! achtung sub schon umgesetzt
      switch(sub)
      {
        case 2: //get mid
          for(uint8_t i=0; i<4; i++)midVals[i] = anaIn(i);
          beepKey();
          break;
        case 3:
          for(uint8_t i=0; i<7; i++)
            if(abs(loVals[i]-hiVals[i])>50) {
              g_eeGeneral.calibMid[i]  = midVals[i];
              int16_t v = midVals[i] - loVals[i];
              g_eeGeneral.calibSpanNeg[i] = v - v/64;
              v = hiVals[i] - midVals[i];
              g_eeGeneral.calibSpanPos[i] = v - v/64;
            }
          int16_t sum=0;
          for(uint8_t i=0; i<12;i++) sum+=g_eeGeneral.calibMid[i];
          g_eeGeneral.chkSum = sum;
          eeDirty(EE_GENERAL); //eeWriteGeneral();
          beepKey();
          break;
      }
      break;
  }
  for(uint8_t i=1; i<4; i++)
  {
    uint8_t y=i*FH+FH;
    lcd_putsnAtt( 0, y,PSTR("SetMid SetSpanDone   ")+7*(i-1),7,
                    sub==i ? INVERS : 0);
  }
  for(uint8_t i=0; i<7; i++)
  {
    uint8_t y=i*FH+0;
    lcd_puts_P( 11*FW,  y+1*FH, PSTR("<    >"));
    lcd_outhex4( 8*FW-3,y+1*FH, sub==2 ? loVals[i]  : g_eeGeneral.calibSpanNeg[i]);
    lcd_outhex4(12*FW,  y+1*FH, sub==1 ? anaIn(i) : (sub==2 ? midVals[i] : g_eeGeneral.calibMid[i]));
    lcd_outhex4(17*FW,  y+1*FH, sub==2 ? hiVals[i]  : g_eeGeneral.calibSpanPos[i]);
  }

}
void menuProcDiagAna(uint8_t event)
{
  static MState2 mstate2;
  TITLE("ANA");
  MSTATE_CHECK_V(6,menuTabDiag,2);
  int8_t  sub    = mstate2.m_posVert ;

  for(uint8_t i=0; i<8; i++)
  {
    uint8_t y=i*FH;
    lcd_putsn_P( 4*FW, y,PSTR("A1A2A3A4A5A6A7A8")+2*i,2);
    //lcd_outhex4( 8*FW, y,g_anaIns[i]);
    lcd_outhex4( 8*FW, y,anaIn(i));
    if(i<7){
      //int16_t v = g_anaIns[i];
      int16_t v = anaIn(i) - g_eeGeneral.calibMid[i];
      v =  v*50/max(1, (v > 0 ? g_eeGeneral.calibSpanPos[i] :  g_eeGeneral.calibSpanNeg[i])/2);
      lcd_outdez(17*FW, y, v);
        //lcd_outdez(17*FW, y, (v-g_eeGeneral.calibMid[i])*50/ max(1,g_eeGeneral.calibSpan[i]/2));
    }
    if(i==7){
      putsVBat(13*FW,y,false,(sub==1 ? INVERS : 0)|PREC1);
    }
  }
  if(sub==1){
   CHECK_INCDEC_H_GENVAR(event, g_eeGeneral.vBatCalib, -127, 127);
  }

}

void menuProcDiagKeys(uint8_t event)
{
  static MState2 mstate2;
  TITLE("DIAG");
  MSTATE_CHECK_V(5,menuTabDiag,1);
  uint8_t x;

  x=7*FW;
  for(uint8_t i=0; i<9; i++)
  {
    uint8_t y=i*FH; //+FH;
    if(i>(SW_ID0-SW_BASE_DIAG)) y-=FH; //overwrite ID0
    bool t=keyState((EnumKeys)(SW_BASE_DIAG+i));
    putsDrSwitches(x,y,i+1,0); //ohne off,on
    lcd_putcAtt(x+FW*4+2,  y,t+'0',t ? INVERS : 0);
  }

  x=0;
  for(uint8_t i=0; i<6; i++)
  {
    uint8_t y=(5-i)*FH+2*FH;
    bool t=keyState((EnumKeys)(KEY_MENU+i));
    lcd_putsn_P(x, y,PSTR(" Menu Exit Down   UpRight Left")+5*i,5);
    lcd_putcAtt(x+FW*5+2,  y,t+'0',t);
  }


  x=14*FW;
  lcd_putsn_P(x, 3*FH,PSTR("Trim- +"),7);
  for(uint8_t i=0; i<4; i++)
  {
    uint8_t y=i*FH+FH*4;
    //lcd_putsn_P(x+7, y,PSTR("TR_LH-TR_LH+TR_LV-TR_LV+TR_RV-TR_RV+TR_RH-TR_RH+")+6*i,6);
    lcd_img(    x,       y, sticks,i,0);
    bool tm=keyState((EnumKeys)(TRM_BASE+2*i));
    bool tp=keyState((EnumKeys)(TRM_BASE+2*i+1));
    lcd_putcAtt(x+FW*4,  y, tm+'0',tm ? INVERS : 0);
    lcd_putcAtt(x+FW*6,  y, tp+'0',tp ? INVERS : 0);
  }
}
void menuProcDiagVers(uint8_t event)
{
  static MState2 mstate2;
  TITLE("VERSION");
  MSTATE_CHECK_V(4,menuTabDiag,1);
  lcd_puts_P(0, 2*FH,stamp4 );
  lcd_puts_P(0, 3*FH,stamp1 );
  lcd_puts_P(0, 4*FH,stamp2 );
  lcd_puts_P(0, 5*FH,stamp3 );
}

void menuProcPPMIn(uint8_t event)
{
  static MState2 mstate2;
  TITLE("PPMIN");
  MSTATE_TAB = { 1,4};
  MSTATE_CHECK_VxH(3,menuTabDiag,1+1);
  uint8_t y;
  uint8_t edit = (mstate2.m_posVert==1);

  switch(event)
  {
    case EVT_ENTRY:
      s_editMode = false;
      //mstate2.m_posHorz = -1;
      break;
  }

  y    = 3*FH;
  lcd_putsnAtt(  0*FW, y, PSTR("Cal"),3, edit ? INVERS : 0);
  for(uint8_t i=0; i<8; i++)
  {
    uint8_t x = i<4 ? (i*8+16)*FW/2 : ((i-4)*8+16)*FW/2;
    lcd_outdezAtt( x, i<4 ? y : y+FH, (g_ppmIns[i]-g_eeGeneral.ppmInCalib[i])*2,PREC1 );
  }
  if(edit)
  {
    if(event==EVT_KEY_FIRST(KEY_MENU)){
      memcpy(g_eeGeneral.ppmInCalib,g_ppmIns,sizeof(g_eeGeneral.ppmInCalib));
      eeDirty(EE_GENERAL);
      beepKey();
    }
  }

}
void menuProcSetup1(uint8_t event)
{
  static MState2 mstate2;
  TITLE("SETUP OPTS");
  MSTATE_CHECK_V(2,menuTabDiag,1+4);
  int8_t  sub    = mstate2.m_posVert-1 ;
  for(uint8_t i=0; i<4; i++){
    uint8_t y=i*FH+2*FH;
    uint8_t attr = sub==i ? INVERS : 0;
    lcd_putsnAtt( FW*7,y,PSTR("THR Warn"
                              "SW  Warn"
                              "Mem Warn"
                              "Beeper  ")+i*8,8,0);
    switch(i){
      case 0:
      case 1:
      case 2:
        {
          uint8_t bit = 1<<i;
          bool    val = !(g_eeGeneral.warnOpts & bit);
          lcd_putsAtt( FW*3, y, val ? PSTR("ON"): PSTR("OFF"),attr);
          if(attr)  val = checkIncDec_hg( event, val, 0, 1); //!! bitfield
          g_eeGeneral.warnOpts |= bit;
          if(val) g_eeGeneral.warnOpts &= ~bit;
          break;
        }
      case 3:
        uint8_t bits = 3<<i;
        uint8_t val = (g_eeGeneral.warnOpts & bits)>>3;
        lcd_outdezAtt( FW*4, y, val,attr);
        if(attr)  val = checkIncDec_hg( event, val, 0, 3); //!! bitfield
        g_eeGeneral.warnOpts = (g_eeGeneral.warnOpts & ~bits) | (val<<3);
        break;
    }
  }
}
void menuProcSetup0(uint8_t event)
{
  static MState2 mstate2;
  TITLE("SETUP BASIC");
  MSTATE_CHECK_V(1,menuTabDiag,1+6);
  int8_t  sub    = mstate2.m_posVert-1 ;
  uint8_t y=FH;
  lcd_outdezAtt(4*FW,y,g_eeGeneral.contrast,sub==0 ? INVERS : 0);
  if(sub==0){
    CHECK_INCDEC_H_GENVAR(event, g_eeGeneral.contrast, 20, 45);
    lcdSetRefVolt(g_eeGeneral.contrast);
  }
  lcd_puts_P( 6*FW, y,PSTR("CONTRAST"));
  y+=FH;

  lcd_outdezAtt(4*FW,y,g_eeGeneral.vBatWarn,(sub==1 ? INVERS : 0)|PREC1);
  if(sub==1){
    CHECK_INCDEC_H_GENVAR(event, g_eeGeneral.vBatWarn, 50, 100); //5-10V
  }
  lcd_puts_P( 4*FW, y,PSTR("V BAT WARNING"));
  y+=FH;

  lcd_outdezAtt(4*FW,y,g_eeGeneral.inactivityTimer*10,(sub==2 ? INVERS : 0));
  if(sub==2){
    CHECK_INCDEC_H_GENVAR(event, g_eeGeneral.inactivityTimer, 0, 30); //0..300minutes
  }
  lcd_puts_P( 4*FW, y,PSTR("m Inactivity Alrm"));
  y+=FH;

  lcd_putsnAtt(1*FW, y, PSTR("OFF ON")+3*g_eeGeneral.throttleReversed,3,(sub==3 ? INVERS:0));
  if(sub==3){
    CHECK_INCDEC_H_GENVAR(event, g_eeGeneral.throttleReversed, 0, 1);
  }
  lcd_puts_P( 6*FW, y,PSTR("Throttle Rev"));
  y+=FH;

  putsDrSwitches(0*FW,y,g_eeGeneral.lightSw,sub==4 ? INVERS : 0);
  if(sub==4){
    CHECK_INCDEC_H_GENVAR(event, g_eeGeneral.lightSw, -MAX_DRSWITCH, MAX_DRSWITCH); //5-10V
  }
  lcd_puts_P( 6*FW, y,PSTR("LIGHT"));


  y+=FH;
  lcd_putsAtt( 1*FW, y, PSTR("Mode"),0);//sub==3?INVERS:0);
  lcd_putcAtt( 3*FW, y+FH, '1'+g_eeGeneral.stickMode,sub==5?INVERS:0);
  for(uint8_t i=0; i<4; i++)
  {
    lcd_img(    (6+4*i)*FW, y,   sticks,i,0);
    putsChnRaw( (6+4*i)*FW, y+FH,i+1,0);//sub==3?INVERS:0);
  }
  if(sub==5){
    CHECK_INCDEC_H_GENVAR(event,g_eeGeneral.stickMode,0,3);
  }
}

uint16_t s_timeCumTot;
uint16_t s_timeCumAbs;  //laufzeit in 1/16 sec
uint16_t s_timeCumSw;  //laufzeit in 1/16 sec
uint16_t s_timeCumThr;  //gewichtete laufzeit in 1/16 sec
uint16_t s_timeCum16ThrP; //gewichtete laufzeit in 1/16 sec
uint8_t  s_timerState;
#define TMR_OFF     0
#define TMR_RUNNING 1
#define TMR_BEEPING 2
#define TMR_STOPPED 3
int16_t  s_timerVal;
void timer(uint8_t val)
{
  int8_t tm = g_model.tmrMode;
  static uint16_t s_time;
  static uint16_t s_cnt;
  static uint16_t s_sum;
  static uint8_t sw_toggled;

  if(abs(tm)>50){ //toggeled switch
    static uint8_t lastSwPos;
    uint8_t swPos = getSwitch(tm>0 ? tm-50 : tm+50 ,0);
    if(swPos && !lastSwPos)  sw_toggled = !sw_toggled;  //if switcdh is flipped first time -> change counter state
    lastSwPos = swPos;
  }

  s_cnt++;
  s_sum+=val;
  if((g_tmr10ms-s_time)<100) //1 sec
    return;
  s_time += 100;
  val     = s_sum/s_cnt;
  s_sum  -= val*s_cnt; //rest
  s_cnt   = 0;

  if(abs(tm)<4) sw_toggled = false; // not switch - sw timer off
  else if(abs(tm)<50) sw_toggled = getSwitch((tm>0 ? tm-10 : tm+10) ,0); //normal switch

  s_timeCumTot               += 1;
  s_timeCumAbs               += 1;
  if(val) s_timeCumThr       += 1;
  if(sw_toggled) s_timeCumSw += 1;
  s_timeCum16ThrP            += val/2;

  s_timerVal = g_model.tmrVal;
  switch(g_model.tmrMode)
  {
    case TMRMODE_NONE:
      s_timerState = TMR_OFF;
      return;
    case TMRMODE_THR_REL:
      s_timerVal -= s_timeCum16ThrP/16;
      break;
    case TMRMODE_THR:
      s_timerVal -= s_timeCumThr;
      break;
    case TMRMODE_ABS:
      s_timerVal -= s_timeCumAbs;
      //s_timeCum16 += 16;
      break;
    default: //switch
      s_timerVal -= s_timeCumSw;
      break;
  }
  switch(s_timerState)
  {
    case TMR_OFF:
      if(g_model.tmrMode != TMRMODE_NONE) s_timerState=TMR_RUNNING;
      break;
    case TMR_RUNNING:
      if(s_timerVal<=0 && g_model.tmrVal) s_timerState=TMR_BEEPING;
      break;
    case TMR_BEEPING:
      if(s_timerVal <= -MAX_ALERT_TIME)   s_timerState=TMR_STOPPED;
      if(g_model.tmrVal == 0)             s_timerState=TMR_RUNNING;
      break;
    case TMR_STOPPED:
      break;
  }

  if(s_timerState==TMR_BEEPING){
    static int16_t last_tmr;
    if(last_tmr != s_timerVal){
      last_tmr   = s_timerVal;
      beepWarn();
    }
  }
}


#define MAXTRACE 120
uint8_t s_traceBuf[MAXTRACE];
uint16_t s_traceWr;
uint16_t s_traceCnt;
void trace(uint8_t val)
{
  timer(val);
  static uint16_t s_time;
  static uint16_t s_cnt;
  static uint16_t s_sum;
  s_cnt++;
  s_sum+=val;
  if((g_tmr10ms-s_time)<1000) //10 sec
    return;
  s_time= g_tmr10ms;
  val   = s_sum/s_cnt;
  s_sum = 0;
  s_cnt = 0;


  s_traceCnt++;
  s_traceBuf[s_traceWr++] = val;
  if(s_traceWr>=MAXTRACE) s_traceWr=0;
}


uint16_t g_tmr1Latency_max;
uint16_t g_tmr1Latency_min = 0x7ff;
uint16_t g_timeMain;
void menuProcStatistic2(uint8_t event)
{
  TITLE("STAT2");
  switch(event)
  {
    case EVT_KEY_FIRST(KEY_MENU):
      g_tmr1Latency_min = 0x7ff;
      g_tmr1Latency_max = 0;
      g_timeMain    = 0;
      beepKey();
      break;
    case EVT_KEY_FIRST(KEY_DOWN):
      chainMenu(menuProcStatistic);
      break;
    case EVT_KEY_FIRST(KEY_UP):
    case EVT_KEY_FIRST(KEY_EXIT):
      chainMenu(menuProc0);
      break;
  }
  lcd_puts_P( 0*FW,  1*FH, PSTR("tmr1Lat max    us"));
  lcd_outdez(14*FW , 1*FH, g_tmr1Latency_max/2 );
  lcd_puts_P( 0*FW,  2*FH, PSTR("tmr1Lat min    us"));
  lcd_outdez(14*FW , 2*FH, g_tmr1Latency_min/2 );
  lcd_puts_P( 0*FW,  3*FH, PSTR("tmr1 Jitter    us"));
  lcd_outdez(14*FW , 3*FH, (g_tmr1Latency_max - g_tmr1Latency_min) /2 );
  lcd_puts_P( 0*FW,  4*FH, PSTR("tmain          ms"));
  lcd_outdez(14*FW , 4*FH, g_timeMain/16 );
}

void menuProcStatistic(uint8_t event)
{
  TITLE("STAT");
  switch(event)
  {
    case EVT_KEY_FIRST(KEY_UP):
      chainMenu(menuProcStatistic2);
      break;
    case EVT_KEY_FIRST(KEY_DOWN):
    case EVT_KEY_FIRST(KEY_EXIT):
      chainMenu(menuProc0);
      break;
  }

  lcd_puts_P(  1*FW, FH*1, PSTR("TME"));
  putsTime(    4*FW, FH*1, s_timeCumAbs, 0, 0);
  lcd_puts_P( 17*FW, FH*1, PSTR("TSW"));
  putsTime(   10*FW, FH*1, s_timeCumSw,      0, 0);

  lcd_puts_P(  1*FW, FH*2, PSTR("THS"));
  putsTime(    4*FW, FH*2, s_timeCumThr, 0, 0);
  lcd_puts_P( 17*FW, FH*2, PSTR("TH%"));
  putsTime(   10*FW, FH*2, s_timeCum16ThrP/16, 0, 0);

  lcd_puts_P( 17*FW, FH*0, PSTR("TOT"));
  putsTime(   10*FW, FH*0, s_timeCumTot, 0, 0);

  uint16_t traceRd = s_traceCnt>MAXTRACE ? s_traceWr : 0;
  uint8_t x=5;
  uint8_t y=60;
  lcd_hline(x-3,y,120+3+3);
  lcd_vline(x,y-32,32+3);

  for(uint8_t i=0; i<120; i+=6)
  {
    lcd_vline(x+i+6,y-1,3);
  }
  for(uint8_t i=1; i<=120; i++)
  {
    lcd_vline(x+i,y-s_traceBuf[traceRd],s_traceBuf[traceRd]);
    traceRd++;
    if(traceRd>=MAXTRACE) traceRd=0;
    if(traceRd==s_traceWr) break;
  }

}

extern volatile uint16_t captureRing[16];


void menuProc0(uint8_t event)
{
  static uint8_t   sub;
  static MenuFuncP s_lastPopMenu[2];
  static uint8_t trimSwLock;

  switch(event)
  {
    case  EVT_KEY_LONG(KEY_MENU):// go to last menu
      pushMenu(lastMenu);
      killEvents(event);
      break;
    case EVT_KEY_FIRST(KEY_RIGHT):
      if(getEventDbl(event)==2 && s_lastPopMenu[1]){
        pushMenu(s_lastPopMenu[1]);
        break;
      }
      if(sub<1) {
        sub=sub+1;
        beepKey();
      }
      break;
    case EVT_KEY_LONG(KEY_RIGHT):
      pushMenu(menuProcModelSelect);//menuProcExpoAll);
      killEvents(event);
      break;
    case EVT_KEY_FIRST(KEY_LEFT):
      if(getEventDbl(event)==2 && s_lastPopMenu[0]){
        pushMenu(s_lastPopMenu[0]);
        break;
      }
      if(sub>0) {
        sub=sub-1;
        beepKey();
      }
      break;
    case EVT_KEY_LONG(KEY_LEFT):
      pushMenu(menuProcSetup0);
      killEvents(event);
      break;
#define MAX_VIEWS 3
    case EVT_KEY_BREAK(KEY_UP):
      g_eeGeneral.view++;
      if(g_eeGeneral.view>=MAX_VIEWS) g_eeGeneral.view=0;
      eeDirty(EE_GENERAL);
      beepKey();
      break;
    case EVT_KEY_BREAK(KEY_DOWN):
      if(g_eeGeneral.view>0)
        g_eeGeneral.view--;
      else
        g_eeGeneral.view = MAX_VIEWS-1;
      eeDirty(EE_GENERAL);
      beepKey();
      break;
    case EVT_KEY_LONG(KEY_UP):
      chainMenu(menuProcStatistic);
      killEvents(event);
      break;
    case EVT_KEY_LONG(KEY_DOWN):
      chainMenu(menuProcStatistic2);
      killEvents(event);
      break;
    case EVT_KEY_FIRST(KEY_EXIT):
      if(s_timerState==TMR_BEEPING) {
        s_timerState = TMR_STOPPED;
        beepKey();
      }
      break;
    case EVT_KEY_LONG(KEY_EXIT):
      s_timerState = TMR_OFF; //is changed to RUNNING dep from mode
      s_timeCumAbs=0;
      s_timeCumThr=0;
      s_timeCumSw=0;
      s_timeCum16ThrP=0;
      beepKey();
      break;
    case EVT_ENTRY_UP:
      s_lastPopMenu[sub] = lastPopMenu();
    case EVT_ENTRY:
      killEvents(KEY_EXIT);
      killEvents(KEY_UP);
      killEvents(KEY_DOWN);
      trimSwLock = true;
      break;
  }

  if(getSwitch(g_model.trimSw,0) && !trimSwLock) setStickCenter();
  trimSwLock = getSwitch(g_model.trimSw,0);

  uint8_t x=FW*2;
    uint8_t att = (g_vbat100mV < g_eeGeneral.vBatWarn ? BLINK : 0) | DBLSIZE;
    for(uint8_t i=0;i<sizeof(g_model.name);i++)
      lcd_putcAtt(x+i*2*FW-i-2, 0*FH, g_model.name[i],DBLSIZE);
    putsVBat(x-1*FW,2*FH,true, att);
    lcd_putcAtt(x+4*FW, 3*FH, 'V',0);
    lcd_hline(x+2*FW,4*FH-4,2);
    lcd_hline(x+2*FW,4*FH-3,2);

  if(s_timerState != TMR_OFF){
    uint8_t att = DBLSIZE | (s_timerState==TMR_BEEPING ? BLINK : 0);
    putsTime(x+9*FW, FH*2, s_timerVal, att,att);

    int8_t tm = g_model.tmrMode;
    if(abs(tm)<10)
      lcd_putsnAtt(x+8*FW-FW/2, FH*3, PSTR("OFFABSTHSTH%")+3*tm,3,0);
    else if(abs(tm)<50)
      putsDrSwitches(x+7*FW-FW/2, FH*3,tm>0 ? tm-10 : tm+10,0);//normal on-off
    else {
      putsDrSwitches(x+6*FW-FW/2, FH*3,tm>0 ? tm-50 : tm+50,0);//momentary on-off
      lcd_putcAtt(x+10*FW-FW/2,  FH*3,'m',0);
    }

  }

  lcd_putsnAtt(x+4*FW,     2*FH,PSTR("ExpExFFneMedCrs")+3*g_model.trimInc,3, 0);
  lcd_putsnAtt(x+8*FW-FW/2,2*FH,PSTR("   TTm")+3*g_model.thrTrim,3, 0);


  //trim sliders
  for(uint8_t i=0; i<4; i++)
  {
#define TL 27
    //                        LH LV RV RH
    static uint8_t x[4]    = {128*1/4+2, 4, 128-4, 128*3/4-2};
    static uint8_t vert[4] = {0,1,1,0};
    uint8_t xm,ym;
    xm=x[i];
    int8_t val = max((int8_t)-(TL+1),min((int8_t)(TL+1),(int8_t)(g_model.trim[i]/4)));
    if(vert[i]){
      ym=31;
      lcd_vline(xm,   ym-TL, TL*2);

      if(((g_eeGeneral.stickMode&1) != (i&1)) || !(g_model.thrTrim)){
        lcd_vline(xm-1, ym-1,  3);
        lcd_vline(xm+1, ym-1,  3);
      }
      //lcd_hline(xm-1, ym,     3);
      ym -= val;
    }else{
      ym=60;
      lcd_hline(xm-TL,ym,    TL*2);
      lcd_hline(xm-1, ym-1,  3);
      lcd_hline(xm-1, ym+1,  3);
      //lcd_vline(xm,   ym-1,     3);
      xm += val;
    }

    //value marker
    //#define MW 7
    //lcd_vline(xm-MW/2,ym-MW/2,MW);
    //lcd_hline(xm-MW/2,ym+MW/2,MW);
    //lcd_vline(xm+MW/2,ym-MW/2,MW);
    //lcd_hline(xm-MW/2,ym-MW/2,MW);
    DO_SQUARE(xm,ym,7)
  }

  if(g_eeGeneral.view!=2) {
   for(uint8_t i=0; i<8; i++)
   {
    uint8_t x0,y0;
    int16_t val = g_chans512[i]/2;
    //val += g_model.limitData[i].revert ? g_model.limitData[i].offset : -g_model.limitData[i].offset;
    switch(g_eeGeneral.view)
    {
      case 0:
        x0 = (i%4*9+3)*FW/2;
        y0 = i/4*FH+40;
        // *1000/512 = x*2 - x/16 + x/64
#define GPERC(x)  (x*2 - x/16 + x/64)
        lcd_outdezAtt( x0+4*FW , y0, GPERC(val),PREC1 );
        break;
      case 1:
#define WBAR2 (50/2)
#define GPERC2(x) GPERC(x/2)
        x0       = i<4 ? 128/4+4 : 128*3/4-4;
        y0       = 38+(i%4)*5;
        int8_t l = (abs(GPERC2(val))+WBAR2/2) * WBAR2 / 512;
        if(l>WBAR2)  l =  WBAR2;  // prevent bars from going over the end - comment for debugging

        lcd_hlineStip(x0-WBAR2,y0,WBAR2*2+1,0x55);
        lcd_vline(x0,y0-2,5);
        if(g_chans512[i]>0){
          x0+=1;
        }else{
          x0-=l;
        }
        lcd_hline(x0,y0+1,l);
        lcd_hline(x0,y0-1,l);
        break;
    }
   }
  }
  else {
    #define BOX_WIDTH     23
    #define BAR_HEIGHT    (BOX_WIDTH-1l)
    #define MARKER_WIDTH  5
    #define SCREEN_WIDTH  128
    #define SCREEN_HEIGHT 64
    #define BOX_LIMIT     (BOX_WIDTH-MARKER_WIDTH)
    #define LBOX_CENTERX  (  SCREEN_WIDTH/4 + 10)
    #define LBOX_CENTERY  (SCREEN_HEIGHT-9-BOX_WIDTH/2)
    #define RBOX_CENTERX  (3*SCREEN_WIDTH/4 - 10)
    #define RBOX_CENTERY  (SCREEN_HEIGHT-9-BOX_WIDTH/2)

    DO_SQUARE(LBOX_CENTERX,LBOX_CENTERY,BOX_WIDTH);
    DO_SQUARE(RBOX_CENTERX,RBOX_CENTERY,BOX_WIDTH);

    DO_CROSS(LBOX_CENTERX,LBOX_CENTERY,3)
    DO_CROSS(RBOX_CENTERX,RBOX_CENTERY,3)
    DO_SQUARE(LBOX_CENTERX+(calibratedStick[0]*BOX_LIMIT/1024), LBOX_CENTERY-(calibratedStick[1]*BOX_LIMIT/1024), MARKER_WIDTH)
    DO_SQUARE(RBOX_CENTERX+(calibratedStick[3]*BOX_LIMIT/1024), RBOX_CENTERY-(calibratedStick[2]*BOX_LIMIT/1024), MARKER_WIDTH)

    V_BAR(SCREEN_WIDTH/2-5,SCREEN_HEIGHT-10,((calibratedStick[4]+RESX)*BAR_HEIGHT/(RESX*2))+1l) //P1
    V_BAR(SCREEN_WIDTH/2  ,SCREEN_HEIGHT-10,((calibratedStick[6]+RESX)*BAR_HEIGHT/(RESX*2))+1l) //P2
    V_BAR(SCREEN_WIDTH/2+5,SCREEN_HEIGHT-10,((calibratedStick[5]+RESX)*BAR_HEIGHT/(RESX*2))+1l) //P3

    for(int8_t i=0; i<3; i++)  {
      //uint8_t y=i*FH+4*FH; //+FH;
      //bool t=keyState((EnumKeys)(SW_BASE_DIAG+i));
      lcd_putsnAtt(2*FW-2,i*FH+4*FH,PSTR(SWITCHES_STR)+3*i,3,getSwitch(i+1, 0) ? INVERS : 0);
    }

    for(int8_t i=6; i<9; i++)  {
      //uint8_t y=(12-i)*FH; //+FH;
      //bool t=keyState((EnumKeys)(SW_BASE_DIAG+i));
      lcd_putsnAtt(17*FW-1,12*FH-i*FH,PSTR(SWITCHES_STR)+3*i,3,getSwitch(i+1, 0) ? INVERS : 0);
    }
  }

}



int16_t intpol(int16_t x, uint8_t idx) // -100, -75, -50, -25, 0 ,25 ,50, 75, 100
{
#define D9 (RESX * 2 / 8)
#define D5 (RESX * 2 / 4)
  bool    cv9 = idx >= MAX_CURVE5;
  int8_t *crv = cv9 ? g_model.curves9[idx-MAX_CURVE5] : g_model.curves5[idx];
  int16_t erg;

  x+=RESXu;
  if(x < 0) {
    erg = crv[0] * (RESX/2);
  } else if(x >= (RESX*2)) {
    erg = crv[(cv9 ? 8 : 4)] * (RESX/2);
  } else {
    int16_t a,dx;
    if(cv9){
      a   = (uint16_t)x / D9;
      dx  =((uint16_t)x % D9) * 2;
    } else {
      a   = (uint16_t)x / D5;
      dx  = (uint16_t)x % D5;
    }
    erg  = (int16_t)crv[a]*(D5-dx) + (int16_t)crv[a+1]*(dx);
  }
  return erg / 50; // 100*D5/RESX;
}

uint16_t pulses2MHz[60];

void perOut(int16_t *chanOut, uint8_t init, uint8_t zeroInput)
{
  static int16_t  anas  [NUM_XCHNRAW];
  static int32_t  chans [NUM_CHNOUT];          // Outputs + intermidiates
  static uint32_t inacCounter;
  static uint16_t inacSum;

         int16_t  trimA    [4];

  if(g_eeGeneral.inactivityTimer) {
    inacCounter++;
    uint16_t tsum = 0;
    for(uint8_t i=0;i<4;i++) tsum += anas[i]/4;//div 4 -> reduce sensitivity
    if(tsum!=inacSum){
      inacSum = tsum;
      inacCounter=0;
    }                                                //   s  m 10min  - using 97 instead of 100 for accuracy
    if(inacCounter>((uint32_t)g_eeGeneral.inactivityTimer*97*60*10)) beepErr();
  }

  for(uint8_t i=0;i<7;i++){        // calc Sticks

    //Normalization  [0..1024] ->   [-512..512]

    int16_t v = anaIn(i);
    v -= g_eeGeneral.calibMid[i];
    v  =  v * (int32_t)RESX /  (max((int16_t)100,(v>0 ?
                                     g_eeGeneral.calibSpanPos[i] :
                                     g_eeGeneral.calibSpanNeg[i])));
    if(v <= -RESX) v = -RESX;
    if(v >=  RESX) v =  RESX;
    calibratedStick[i] = v; //for show in expo


    if(i<4) { //only do this for sticks
      uint8_t expoDrOn = GET_DR_STATE(i);
      uint8_t stkDir = v>0 ? DR_RIGHT : DR_LEFT;

      if(IS_THROTTLE(i) && g_model.thrExpo){
        v  = 2*expo((v+RESX)/2,g_model.expoData[i].expo[expoDrOn][DR_EXPO][DR_RIGHT]);
        stkDir = DR_RIGHT;
      }
      else
        v  = expo(v,g_model.expoData[i].expo[expoDrOn][DR_EXPO][stkDir]);
      int32_t x = (int32_t)v * (g_model.expoData[i].expo[expoDrOn][DR_WEIGHT][stkDir]+100)/100;
      v = (int16_t)x;
      if (IS_THROTTLE(i) && g_model.thrExpo) v -= RESX;

      //trace and trim throttle
      int32_t vv = 2*RESX;
      if(IS_THROTTLE(i))  //stickMode=0123 -> thr=2121
      {
        trace((g_eeGeneral.throttleReversed ? RESX-v : v+RESX) / 32); //trace thr 0..32  (/32)
        if(g_model.thrTrim) vv = (g_eeGeneral.throttleReversed) ?
                                 ((int32_t)g_model.trim[i]-125)*(RESX+v)/(2*RESX) :
                                 ((int32_t)g_model.trim[i]+125)*(RESX-v)/(2*RESX);
      }

    //trim
      trimA[i] = (vv==2*RESX) ? g_model.trim[i] : (int16_t)vv; //    if throttle trim -> trim low end
    }
    anas[i] = v; //10+1 Bit
  }

  anas[MIX_MAX-1]  = RESX;     // MAX
  anas[MIX_FULL-1] = RESX;     // FULL
  for(uint8_t i=MIX_FULL;i<(MIX_FULL+NUM_PPM);i++)          anas[i] = g_ppmIns[i-MIX_FULL] - g_eeGeneral.ppmInCalib[i-MIX_FULL]; //add ppm channels
  for(uint8_t i=MIX_FULL+NUM_PPM;i<NUM_XCHNRAW;i++)  anas[i] = chans[i-MIX_FULL-NUM_PPM]; //other mixes previous outputs

  memset(chans,0,sizeof(chans));        // All outputs to 0

  if(zeroInput)
    for(uint8_t i=0;i<4;i++)
      if(!IS_THROTTLE(i)) {
        anas[i]  = 0;
        trimA[i] = 0;
      }

    //========== MIXER LOOP ===============
    for(uint8_t i=0;i<MAX_MIXERS;i++){
      MixData &md = g_model.mixData[i];

      if((md.destCh==0) || (md.destCh>NUM_CHNOUT)) break;

      //Notice 0 = NC switch means not used -> always on line

      int16_t v  = 0;
      if(!getSwitch(md.swtch,1)){
        if(md.srcRaw!=MIX_MAX && md.srcRaw!=MIX_FULL) continue;// if not MAX or FULL - next loop
        if(md.mltpx==MLTPX_REP && md.srcRaw==MIX_MAX) continue; // if MAX and switch is off and REPLACE then off
        v = (md.srcRaw == MIX_FULL ? -RESX : 0); // switch is off and it is either MAX=0 or FULL=-512
      }
      else
        v = anas[md.srcRaw-1]; //Switch is on. MAX=FULL=512 or value.

      //========== INPUT OFFSET ===============
      if(md.sOffset) v += (int16_t)md.sOffset*5 + md.sOffset/8;

      //========== DELAY and PAUSE ===============
      if (md.speedUp || md.speedDown || md.delayUp || md.delayDown)  // there are delay values
      {
        static int16_t sDelay[MAX_MIXERS];
        static int16_t act   [MAX_MIXERS];
        static bool    swtch [MAX_MIXERS];

        if(init) act[i]=v*32;

        int16_t diff = v-act[i]/32;
        if(abs(diff)<4) diff=0;
        if(diff>0) swtch[i] = true;
        if(diff<0) swtch[i] = false;

        if(!diff)     //set up delay
          sDelay[i] = (swtch[i] ? md.delayUp :  md.delayDown) * 100;
        else if(sDelay[i]){ // perform delay
            sDelay[i]--;
            v = act[i]/32;
            diff = 0;
          }

        if(diff && (md.speedUp || md.speedDown)){
          //rate = steps/sec => 32*1024/100*md.speedUp/Down
          act[i] += diff>0 ? (32768)/((int16_t)100*md.speedUp) : -(32768)/((int16_t)100*md.speedDown);
          v = act[i]/32;
        }
      }

      //========== CURVES ===============
      switch(md.curve){
        case 0:
          break;
        case 1:
          if(md.srcRaw == MIX_FULL) //FUL
          {
            if( v<0 ) v=-RESX;   //x|x>0
            else      v=-RESX+2*v;
          }else{
            if( v<0 ) v=0;   //x|x>0
          }
          break;
        case 2:
          if(md.srcRaw == MIX_FULL) //FUL
          {
            if( v>0 ) v=RESX;   //x|x<0
            else      v=RESX+2*v;
          }else{
            if( v>0 ) v=0;   //x|x<0
          }
          break;
        case 3:       // x|abs(x)
          v = abs(v);
          break;
        case 4:       //f|f>0
          v = v>0 ? RESX : 0;
          break;
        case 5:       //f|f<0
          v = v<0 ? -RESX : 0;
          break;
        case 6:       //f|abs(f)
          v = v>0 ? RESX : -RESX;
          break;
        default:
          v = intpol(v, md.curve - 7);
      }

      //========== TRIM ===============
      if((md.carryTrim==0) && (md.srcRaw>0) && (md.srcRaw<=4)) v += trimA[md.srcRaw-1];  //  0 = Trim ON  =  Default

      //========== MULTIPLEX ===============
      int32_t dv = (int32_t)v*md.weight;
      switch(md.mltpx){
        case MLTPX_REP:
          chans[md.destCh-1] = dv;
          break;
        case MLTPX_MUL:
          chans[md.destCh-1] *= dv/100l;
          chans[md.destCh-1] /= RESXl;
          break;
        default:  // MLTPX_ADD
          chans[md.destCh-1] += dv; //Mixer output add up to the line (dv + (dv>0 ? 100/2 : -100/2))/(100);
          break;
        }
    }

  //========== LIMITS ===============
  for(uint8_t i=0;i<NUM_CHNOUT;i++){
    // chans[i] holds data from mixer.   chans[i] = v*weight => 512*100
    // later we multiply by the limit (up to 100) and then we need to normalize
    // at the end chans[i] = chans[i]/100 =>  -512..512
    // interpolate value with min/max so we get smooth motion from center to stop
    // this limits based on v original values and min=-512, max=512  RESX=512

    int16_t v = 0;
    int16_t lim_p = g_model.limitData[i].max+100;
    int16_t lim_n = g_model.limitData[i].min-100;

    if(chans[i]) {
      v = (chans[i]>0) ? chans[i]*lim_p/5000 : -chans[i]*lim_n/5000; //div by 5000 -> output = -1024..1024
      chans[i] /= 100; // chans back to -512..512
    }

    //impose hard limits
    lim_p = lim_p*10 + lim_p/4;  //lim_p = lim_p*5 + lim_p/8
    lim_n = lim_n*10 + lim_n/4;  //x*10 + x/4 -> f(x=100) = ~1024
    if(v>lim_p) v = lim_p;
    if(v<lim_n) v = lim_n;// absolute limits - do not go over!

    v+=g_model.limitData[i].offset;      //offset after limit.
    if(g_model.limitData[i].revert) v=-v;// finally do the reverse.

    cli();
    chanOut[i] = v; //copy consistent word to int-level
    sei();
  }


}



/******************************************************************************
  the functions below are from int-level
  the functions below are from int-level
  the functions below are from int-level
******************************************************************************/

void setupPulses()
{
  switch(g_model.protocol)
  {
    case PROTO_PPM:
      setupPulsesPPM();
      break;
    case PROTO_SILV_A:
    case PROTO_SILV_B:
    case PROTO_SILV_C:
      setupPulsesSilver();
      break;
    case PROTO_TRACER_CTP1009:
      setupPulsesTracerCtp1009();
      break;
  }
}

void setupPulsesPPM()
{
  //Total frame length = 22.5msec
  //each pulse is 0.7..1.7ms long with a 0.3ms stop tail
  //The pulse ISR is 2mhz that's why everything is multiplied by 2
  uint16_t rest=(22500u-300u*9)*2; //from thus issue 4, 41 yes, I know it's for 8 channels
  uint8_t j=0;
  uint8_t p=8+g_model.ppmNCH*2;
  for(uint8_t i=0;i<p;i++){ //NUM_CHNOUT
    uint16_t v = g_chans512[i] + 1200*2; // we allow the signal to have 2048 steps
    //if(v>1720*2) v = 1720*2; //limit to between 680 - 1720.  Should be enough room
    //if(v<680*2)  v = 680*2; //still not sure I want to limit pulses
    //pulses are limited to -640 .. 640 -> 560 .. 1840 by the limits in perOut()
    rest-=v;
    pulses2MHz[j++]=(g_model.ppmDelay*50+300)*2;
    pulses2MHz[j++]=v;
  }
  pulses2MHz[j++]=(g_model.ppmDelay*50+300)*2;
  if(rest<((22500u-300u*9)*2)) pulses2MHz[j++]=rest;
  pulses2MHz[j++]=0;

}


uint16_t *pulses2MHzPtr;
#define BITLEN (600u*2)
void _send_hilo(uint16_t hi,uint16_t lo)
{
  *pulses2MHzPtr++=hi; *pulses2MHzPtr++=lo;
}
#define send_hilo_silv( hi, lo) _send_hilo( (hi)*BITLEN,(lo)*BITLEN )

void sendBitSilv(uint8_t val)
{
  send_hilo_silv((val)?2:1,(val)?2:1);
}
void send2BitsSilv(uint8_t val)
{
  sendBitSilv(val&2);sendBitSilv(val&1);
}
// _ oder - je 0.6ms  (gemessen 0.7ms)
//
//____-----_-_-_--_--_   -_--__  -_-_-_-_  -_-_-_-_  --__--__-_______
//         trailer        chan     m1         m2
//
//see /home/thus/txt/silverlit/thus.txt
//m1, m2 most significant bit first |m1-m2| <= 9
//chan: 01=C 10=B
//chk = 0 - chan -m1>>2 -m1 -m2>>2 -m2
//<= 500us Probleme
//>= 650us Probleme
//periode orig: 450ms
void setupPulsesSilver()
{
  int8_t chan=1; //chan 1=C 2=B 0=A?

  switch(g_model.protocol)
  {
    case PROTO_SILV_A: chan=0; break;
    case PROTO_SILV_B: chan=2; break;
    case PROTO_SILV_C: chan=1; break;
  }

  int8_t m1 = (uint16_t)(g_chans512[0]+1024)*2 / 256;
  int8_t m2 = (uint16_t)(g_chans512[1]+1024)*2 / 256;
  if (m1 < 0)    m1=0;
  if (m2 < 0)    m2=0;
  if (m1 > 15)   m1=15;
  if (m2 > 15)   m2=15;
  if (m2 > m1+9) m1=m2-9;
  if (m1 > m2+9) m2=m1-9;
  //uint8_t i=0;
  pulses2MHzPtr=pulses2MHz;
  send_hilo_silv(5,1); //idx 0 erzeugt pegel=0 am Ausgang, wird  als high gesendet
  send2BitsSilv(0);
  send_hilo_silv(2,1);
  send_hilo_silv(2,1);

  send2BitsSilv(chan); //chan 1=C 2=B 0=A?
  uint8_t sum = 0 - chan;

  send2BitsSilv(m1>>2); //m1
  sum-=m1>>2;
  send2BitsSilv(m1);
  sum-=m1;

  send2BitsSilv(m2>>2); //m2
  sum-=m2>>2;
  send2BitsSilv(m2);
  sum-=m2;

  send2BitsSilv(sum); //chk

  sendBitSilv(0);
  pulses2MHzPtr--;
  send_hilo_silv(50,0); //low-impuls (pegel=1) ueberschreiben


}



/*
  TRACE CTP-1009
   - = send 45MHz
   _ = send nix
    start1       0      1           start2
  -------__     --_    -__         -----__
   7ms   2     .8 .4  .4 .8         5   2

 frame:
  start1  24Bits_1  start2  24_Bits2

 24Bits_1:
  7 x Bits  Throttle lsb first
  1 x 0

  6 x Bits  rotate lsb first
  1 x Bit   1=rechts
  1 x 0

  4 x Bits  chk5 = nib2 ^ nib4
  4 x Bits  chk6 = nib1 ^ nib3

 24Bits_2:
  7 x Bits  Vorwaets lsb first 0x3f = mid
  1 x 1

  7 x Bits  0x0e lsb first
  1 x 1

  4 x Bits  chk5 = nib2 ^ nib4
  4 x Bits  chk6 = nib1 ^ nib3

 */

#define BIT_TRA (400u*2)
void sendBitTra(uint8_t val)
{
  if(val) _send_hilo( BIT_TRA*1 , BIT_TRA*2 );
  else    _send_hilo( BIT_TRA*2 , BIT_TRA*1 );
}
void sendByteTra(uint8_t val)
{
  for(uint8_t i=0; i<8; i++, val>>=1) sendBitTra(val&1);
}
void setupPulsesTracerCtp1009()
{
  pulses2MHzPtr=pulses2MHz;
  static bool phase;
  if( (phase=!phase) ){
    uint8_t thr = min(127u,(uint16_t)(g_chans512[0]+1024+8) /  16u);
    uint8_t rot;
    if (g_chans512[1] >= 0)
    {
      rot = min(63u,(uint16_t)( g_chans512[1]+16) / 32u) | 0x40;
    }else{
      rot = min(63u,(uint16_t)(-g_chans512[1]+16) / 32u);
    }
#ifdef SIM
    printf("thr %02x  rot %02x\n",thr,rot);
#endif
    sendByteTra(thr);
    sendByteTra(rot);
    uint8_t chk=thr^rot;
    sendByteTra( (chk>>4) | (chk<<4) );
    _send_hilo( 5000*2, 2000*2 );
  }else{
    uint8_t fwd = min(127u,(uint16_t)(g_chans512[2]+1024) /  16u) | 0x80;
#ifdef SIM
    printf("fwd %02x \n",fwd);
#endif
    sendByteTra(fwd);
    sendByteTra(0x8e);
    uint8_t chk=fwd^0x8e;
    sendByteTra( (chk>>4) | (chk<<4) );
    _send_hilo( 7000*2, 2000*2 );
  }
  *pulses2MHzPtr++=0;
  if((pulses2MHzPtr-pulses2MHz) >= (signed)DIM(pulses2MHz)) alert(PSTR("pulse tab overflow"));
}

