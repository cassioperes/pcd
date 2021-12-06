public class TrafficController {
    boolean bridgeOccupied = false;

    public synchronized void enterLeft() {
        //tranca ponte
        try{
            while(bridgeOccupied)
                wait();
            this.bridgeOccupied = true;
        }
        catch(InterruptedException ie){
            System.err.println(ie.toString());
        }
    }

    public synchronized void enterRight() {
        //tranca ponte
        try{
            while(bridgeOccupied)
                wait();
            this.bridgeOccupied = true;
        }
        catch(InterruptedException ie){
            System.err.println(ie.toString());
        }
    }

    public synchronized void leaveLeft() {
        //libera ponte
        notifyAll();
        this.bridgeOccupied = false;
    }

    public synchronized void leaveRight() {
        //libera ponte
        notifyAll();
        this.bridgeOccupied = false;
    }
}