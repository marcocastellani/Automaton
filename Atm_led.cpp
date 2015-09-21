#include <Automaton.h>
#include "Atm_led.h"

Atm_led & Atm_led::begin( int attached_pin )
{ 
	static const state_t state_table[] PROGMEM = {
	/*               ON_ENTER    ON_LOOP  ON_EXIT  EVT_ON_TIMER  EVT_OFF_TIMER  EVT_COUNTER  EVT_ON  EVT_OFF  EVT_BLINK  ELSE */
	/* IDLE      */  ACT_INIT, ATM_SLEEP,      -1,           -1,            -1,          -1,     ON,      -1,     START,   -1, // LED off
	/* ON        */    ACT_ON, ATM_SLEEP,      -1,           -1,            -1,          -1,     -1,    IDLE,     START,   -1, // LED on
	/* START     */    ACT_ON,        -1,      -1,    BLINK_OFF,            -1,          -1,     ON,    IDLE,        -1,   -1, // Start blinking
	/* BLINK_OFF */   ACT_OFF,        -1,      -1,           -1,         START,        IDLE,     ON,      -1,        -1,   -1,
    };
	Machine::begin( state_table, ELSE );
    Machine::msgQueue( messages, MSG_END );
	pin = attached_pin; 
	pinMode( pin, OUTPUT );
	set( on_timer, 500 ); 
	set( off_timer, 500 ); 
	repeat_count = ATM_COUNTER_OFF;
	set( counter, repeat_count );
	return *this;
}

Atm_led & Atm_led::blink( int duration ) 
{
	set( on_timer, duration ); // Time in which led is fully on
	return *this;
}

Atm_led & Atm_led::pause( int duration ) 
{
	set( off_timer, duration ); // Time in which led is fully off
	return *this;
}

Atm_led & Atm_led::fade( int fade ) { return *this; } // Dummy for method compatibility with Atm_fade

Atm_led & Atm_led::repeat( int repeat ) 
{
	repeat_count = repeat >= 0 ? repeat : ATM_COUNTER_OFF;
    set( counter, repeat_count );
	return *this;
}

int Atm_led::event( int id ) 
{
	switch ( id ) {
		case EVT_ON_TIMER :
			return expired( on_timer );        
		case EVT_OFF_TIMER :
			return expired( off_timer );        
		case EVT_COUNTER :
			return expired( counter );
        case EVT_ON :
            return msgRead( MSG_ON );
        case EVT_OFF :
            return msgRead( MSG_OFF );            
        case EVT_BLINK :
            return msgRead( MSG_BLINK );            
	}
	return 0;
}

void Atm_led::action( int id ) 
{
	switch ( id ) {
		case ACT_INIT :
			set( counter, repeat_count );
			digitalWrite( pin, LOW );
			return;
		case ACT_ON :
			decrement( counter );
			digitalWrite( pin, HIGH );
			return;
		case ACT_OFF :
			digitalWrite( pin, LOW );
			return;
	}
}
