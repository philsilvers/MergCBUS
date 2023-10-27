
/********************************************************************************************/
 /* CANBUFFER
  * Can be used in conjunction with CANGATE as an allternative way to produce a 'self consumed' event
  * Outputs of CANGATE can be fed into inputs
  * Setting Event variable 1 to 255 will invert the output event
  * Node Variable 1 is used to either use the same incoming event number  or to multiply the event number by 10
  * Setting NV1 to 1 - same event number in and our e.g. input 210 output 210
  * Setting NV1 to 10 multiplies the event number by 10 e.g. input 210 output 2100
  * Philip Silver MERG Member 4082
  */
/********************************************************************************************/





/*
All Libaries used can be downloaded from my Github account
https://github.com/philsilvers


Pins used for interface chip

           Nano  
INT         D2       
SCK   SCK   D13      
SI    MISO  D11      
SO    MOSI  D12      
CS          D10      
GND         0V       
VCC         5V      

*/
/********************************************************************************************/


/********************************************************************************************/
// Load CBUS Libraries
/********************************************************************************************/


#include <SPI.h> //equired by the CBUS library to communicate to MCP2515 CAN Controller
#include <MergCBUS.h> // Main CBUS Library
#include <Message.h>  // CBUS Message Libary
#include <EEPROM.h> //Required by the CBUS library to read / write Node Identifiction and Node Varaiables


/********************************************************************************************/





/********************************************************************************************/
//CBUS definitions
/********************************************************************************************/
  #define GREEN_LED 5               //MERG Green (SLIM) LED port
  #define YELLOW_LED 4              //MERG Yellow (FLIM) LED port
  #define PUSH_BUTTON 6             //std merg FLIM / SLIM push button
 //#define PUSH_BUTTON1 3          //debug push button
  #define NODE_VARS 1      //sets up number of NVs for module to store variables

  #define NODE_EVENTS 128     //Max Number of supported Events is 255
  #define EVENTS_VARS 20  //number of variables per event Maximum is 20
  #define DEVICE_NUMBERS 0  //number of devices numbers connected to Arduino such as servos, relays etc. Can be used for Short events

/********************************************************************************************/





/********************************************************************************************/
//Variables
/********************************************************************************************/
bool nonInverting = 1;

bool inverting = 1;




/********************************************************************************************/




/********************************************************************************************/
//Create the MERG CBUS object - cbus
/********************************************************************************************/

MergCBUS cbus=MergCBUS(NODE_VARS,NODE_EVENTS,EVENTS_VARS,DEVICE_NUMBERS);

/********************************************************************************************/



MergNodeIdentification MergNode=MergNodeIdentification();


void setup () {




/********************************************************************************************/
//Configuration CBUS data for the node
/********************************************************************************************/

  cbus.getNodeId()->setNodeName("CANBUFF",7);       //node name shows in FCU when first detected set your own name for each module - max 8 characters
  cbus.getNodeId()->setModuleId(129);               //module number - set above 100 to avoid conflict with other MERG CBUS modules
  cbus.getNodeId()->setManufacturerId(0xA5);        //MERG code
  cbus.getNodeId()->setMinCodeVersion(0x61);        //Version a
  cbus.getNodeId()->setMaxCodeVersion(1);           // version 1
  cbus.getNodeId()->setProducerNode(true);          //Produce Node - Set True if Producer Module
  cbus.getNodeId()->setConsumerNode(true);          //Consumer Node = Set True if Consumer Module
  cbus.getNodeId()->setConsumeEvents(false);         // consume own event COE
  cbus.getNodeId()->setTransportType(1);            // 1 CAN  2 ETHERNET 3 MiWi
  cbus.getNodeId()->setBetaRelease(0);              // Beta Release set to zero for release version
  cbus.getNodeId()->setSuportBootLoading(false);    // Allways set false - no bootloading
  cbus.getNodeId()->setCpuManufacturer(2);          //1 MICROCHIP  2  ATMEL  3 ARM


  cbus.setStdNN(999);                               // Node Number in SLIM Mode. The default is 0 for Consumers or 1 - 99 for Producers.
  

 



/********************************************************************************************/
// Set ports and CAN Transport Layer
/********************************************************************************************/

  cbus.setLeds(GREEN_LED,YELLOW_LED);//set the led ports
  cbus.setPushButton(PUSH_BUTTON);//set the push button ports
  cbus.setUserHandlerFunction(&myUserFunc);//function that implements the node logic when recieving events
  cbus.initCanBus(10,CAN_125KBPS,MCP_8MHz,10,200);  //initiate the transport layer. pin=10, rate=125Kbps,10 tries,200 millis between each try
  
  //Note the clock speed 8Mhz. If 16Mhz crystal fitted change above to MCP_16Mhz
/********************************************************************************************/



} // End Of Set Up





/********************************************************************************************/
//  Functions
/********************************************************************************************/



void myUserFunc(Message *msg,MergCBUS *mcbus){
  
  // byte CBUSOpc = msg->getOpc(); // Get The OPCODE from Message
   //int nodeNumber = msg->getNodeNumber(); // Get The Node Number from Message
   int eventNumber = msg->getEventNumber(); // Get The Event Number from Message
   int eventVariable1 = mcbus->getEventVar(msg,1);
       
     
   
   if (mcbus->eventMatch()){  //The recived event has been taught this module

                    if (nonInverting==1){
                     if (mcbus->isAccOn()== true){
                        cbus.sendOnEvent(true, eventNumber);
                        nonInverting=0;
                        }
                    }
                      if (nonInverting == 0){ 
                      if (mcbus->isAccOff()== true) {
                      cbus.sendOffEvent(true, eventNumber);
                      nonInverting=1;
                       }
                     }

             if (eventVariable1 == 255){
               inverting=1;
                    if (mcbus->isAccOn()== true){
                        cbus.sendOffEvent(true, eventNumber);
                        inverting=0;
                        }
                     if (inverting == 0){
                     if (mcbus->isAccOff()== true) {
                      cbus.sendOnEvent(true, eventNumber);
                      inverting=1;
                     }
                     }
       }
                  
            
        } // End OF Recieved Events
          
    
           
  } // end function
                


/*FUNCTIONS FINISH ***********************************************************************************/



void loop() {

    cbus.run();// Run CBUS
    cbus.cbusRead(); // Check CBUS Buffers for any activity

}
