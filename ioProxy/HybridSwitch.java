import java.io.*;
import java.net.*;
import java.util.*;

public class HybridSwitch {

protected Hashtable threads_by_id;

public HybridSwitch( int port ) throws InstantiationException {
	try {
	threads_by_id = new Hashtable();

	ServerSocket serv = new ServerSocket( port );

	for(;;) {
		Socket sock = serv.accept();
		new HybridThread( sock, this );
	}
	} catch( Exception ex ) { 
	    throw new InstantiationException( ex.toString() );
	} 
}

protected void send( String from,
		     String to,
		     String id, 
		     int length, 
		     byte[] data ) {

	HybridThread thr = (HybridThread) threads_by_id.get(to);
	byte[] ackMsg = new byte[2];

	if( thr!=null ) {
		System.out.println( "Sending from ["+from+"] to ["+to+"]" );
		thr.send( from, id, data );
		ackMsg[0] = '1';
	}
	else {
		System.out.println( "Sending from ["+from+"] failed: NO destination" );
		ackMsg[0] = '0';
	}

	thr = (HybridThread) threads_by_id.get(from+"_REG_ACK");

	if(thr != null){
	    ackMsg[1] = '\n';
	    thr.send(from, id, ackMsg);
	}
}

protected void register_thread( String srcID, HybridThread thr ) {
	System.out.println( "Registering subscriber to  ["+srcID+"]" ); 
	if( srcID!=null ) {
		threads_by_id.put( srcID, thr );
	}
}

protected void deregister_thread( String id ) {
	System.out.println( "Deregistering ["+id+"]" ); 
	if( id!=null ) {
		threads_by_id.remove( id );
	}
}

public static void main( String args[] ) {
	try {
		new HybridSwitch( Integer.parseInt(args[0]) );
	} catch( Exception ex ) {
		System.out.println( "Syntax: HybridSwitch [portnum]" ); 
	} 

} 

}
