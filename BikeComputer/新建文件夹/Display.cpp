#include <MicroLCD.h>
#include "Speed.h"
#include "Display.h"




void refresh(){
  // check is clear needed
  



  switch (pageLevel){
  case 0:
    displayInfo();
    break;
  case 1:
    displayMenu();
    break;
  case 2:
    switch(displayPage[pagelevel-1]){
    case 0:
    case 1:
      saveMode = true;
      lcd.setCursor(8,0);
      lcd.print(menu[displayPage[pagelevel-1]]);
      lcd.setCursor(60,4);
      lcd.print(displayPage[pagelevel]);
      break;


    }
    break;
  default:
    ;
  }
  //showCursor();
}

//action left
void actionLeft(){
  if(pagelevel>0){
    pagelevel--;
  }
  needClear=true;
}

//action right
void actionRight(){
  pagelevel++;
  needClear=true;
}

//action up
void actionUp(){
  switch(pagelevel){
  case 0:
    break;
  case 1:
  case 2:
    if(displayPage[pagelevel]>0){
      displayPage[pagelevel]--;

    }
  default: 
    ;
  }
  needClear=true;
}

//action down
void actionDown(){
  switch(pagelevel){
  case 0:
    break;
  case 1:
  case 2:
    if(true){
      displayPage[pagelevel]++;

    }
  default: 
    ;
  }
  needClear=true;
}





















