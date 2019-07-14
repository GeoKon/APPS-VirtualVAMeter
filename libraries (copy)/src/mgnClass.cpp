#include <macros.h>
#include <mgnClass.h>

// Convenient macro to save into the BUF buf if initialized
#define BSAVE( format, ...) if(bpnt) bpnt->add( format, ##__VA_ARGS__ )

	MGN::MGN() 
    { 
        idx = 0;
        bpnt = NULL;
		channel[0]=0;
    }
	// provide a BUF sufficient to accumulate all responses back to caller
    void MGN::init( BUF *bp, const char *chan )
    {
		bpnt = bp;			// set the pointer for all functions
        bpnt->init();		// initialize buffer
		idx = 0;			// number of bytes written
		
		if( *chan )
			sprintf( channel, ":%s", chan );
		else
			channel[0] = 0;
    }
    // ------------------- TABLES -------------------------------------
    void MGN::tableSet( const char *cname, const char *value, const char *descr )
    {
        BSAVE("{TABLE%s|SET|%s", channel, cname);
        if( *value ) BSAVE("|%s", value );
        if( *descr ) BSAVE("|%s", descr );
        BSAVE("}\r\n");
    }
    void MGN::tableSet( const char *cname, float value, const char *descr )
    {
        char s[32];
        sprintf(s,"%.3f", value);
        tableSet( cname, s, descr );
    }
    void MGN::tableSet( const char *cname, int value, const  char *descr )
    {
        char s[32];
        sprintf(s,"%d", value);
        tableSet( cname, s, descr );
    }
    void MGN::tableSet( const char *cname, bool value, const char *descr )
    {
        tableSet( cname, value?"True":"False", descr );
    }
	void MGN::tableGet( const char *cname )
    {
        if( *cname )
			BSAVE("{TABLE%s|GET|%s}\r\n", channel, cname);
		else
			BSAVE("{TABLE%s|GET}\r\n", channel);
    }
    void MGN::tableClear( const char *cname )
    {
        BSAVE("{TABLE%s|CLEAR", channel );
        if( *cname ) BSAVE("|%s", cname );
        BSAVE("}\r\n");
    }
    
    // ------------------- TIME PLOT -----------------------------------
    //
    void MGN::tplotData( const char *series, float value )
    {
        BSAVE("{TIMEPLOT%s|D|%s|T|%.3f}\r\n", channel,series, value );
    }
    void MGN::tplotYRange( float minv, float maxv )
    {
        BSAVE("{TIMEPLOT%s|yrange|%.3f|%.3f}\r\n", channel,minv, maxv );
    }
    void MGN::tplotY2Range( float minv, float maxv )
    {
        BSAVE("{TIMEPLOT%s|y2range|%.3f|%.3f}\r\n", channel,minv, maxv );
    }
    void MGN::tplotSet( const char *prop )
    {
        BSAVE("{TIMEPLOT%s|SET|%s}\r\n", channel, prop); 
    }
    void MGN::tplotClear()
    {
        BSAVE("{TIMEPLOT%s|CLEAR}\r\n", channel); 
    }
    
    // ------------------- CONTROLS -------------------------------------
    //
    void MGN::controlSetText( const char *cname, const char *value ) // text box
    {
//      bpnt->add( "{UI%s|SET|%s.Text=%s}\r\n", channel, cname, value );
		BSAVE("{UI%s|SET|%s.Text=%s}\r\n", channel, cname, value); 
    }
    void MGN::controlSetValue( const char *cname, float value )
    {
        BSAVE("{UI%s|SET|%s.Value=%.3f}\r\n", channel, cname, value); // numeric up/down
    }
    void MGN::controlSetValue( const char *cname, int value ) // progress bar
    {
        BSAVE("{UI%s|SET|%s.Value=%d}\r\n", channel, cname, value);
    }
    void MGN::controlSetCheck( const char *cname, bool onoff )
    {
        BSAVE("{UI%s|SET|%s.Checked=%s}\r\n", channel, cname, onoff?"True":"False" );
    }
    void MGN::controlSetProperty( const char *cnameprop ) // text box
    {
        BSAVE("{UI%s|SET|%s}\r\n", channel,cnameprop); 
    }
    // -------------------- Buffer Management -----------------------------
     
    void MGN::clear()
    {
		bpnt->init();
    }
    char *MGN::getBuf()
    {
    	return bpnt->c_str();
    }
    void MGN::print()
    {
    	bpnt->print();
		if( bpnt->length() > (bpnt->size()-10) )
			PFN("Insufficient buffer size (used=%d, max=%d)", bpnt->length(), bpnt->size() );			
    }

