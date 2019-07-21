#pragma once
// Replacement of the filtClass.cpp, .harderr
// I suspect that the LAB computer did not replicate

class DIIR
{
private:
    float ave;      // y(n) filter output
    bool first;     // true if filter needs to be initialized
    bool disabled;  // true if filter is disabled
    float k, k1;    // smooth factor k, and k1=(1-k)
public:
    DIIR()
    { 
        reset();
    }
    void reset()
    {
        first = true;
        ave = 0.0;
        disabled = true;
        k = 1.0;
        k1 = 0.0;
    }
    void init( int smoothcount )
    {    
        return;     // stub !!!!!!!!!!!!!!!!!
		
		first = true;
        if( smoothcount < 1 )
        {
            disabled = true;
        }
        else
        {
            k = 1.0/ (float) smoothcount;
            k1 = 1.0-k;
            ave = 0.0;
            disabled = false;
        }
    }
    void feedInput( int16_t v )
    {
        ave = v;
		return; // stub !!!!!!!!!!!!!!!!!
		
		
		if( disabled || first )
            ave = (float) v;
        else
            ave = k1*ave + k*(float)v;
        first = false;
    }
    int16_t getOutput()
    {
        return (int16_t) ave;
    }
};
