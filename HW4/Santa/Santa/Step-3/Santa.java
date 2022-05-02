//import com.sun.org.apache.xml.internal.security.utils.HelperNodeList;


public class Santa implements Runnable {

	enum SantaState {SLEEPING, READY_FOR_CHRISTMAS, WOKEN_UP_BY_ELVES, WOKEN_UP_BY_REINDEER};
	private SantaState state;
	private boolean running;
	SantaScenario scenario;

	public Santa(SantaScenario scenario) {
		this.state = SantaState.SLEEPING;
		this.scenario = scenario;
		this.running = true;
	}


	@Override
	public void run() {
		while(this.running) {
			// wait a day...
			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			switch(state) {
			case SLEEPING: // if sleeping, continue to sleep
				break;
			case WOKEN_UP_BY_ELVES:
				// FIXME: help the elves who are at the door and go back to sleep
				for(Elf elf: scenario.elves)
				{
					if (elf.getState() == Elf.ElfState.AT_SANTAS_DOOR)
					{ // check if elf is at door
						elf.setState(Elf.ElfState.WORKING); // solve the elf's problem
						scenario.elvesAtDoor.remove(elf); // send elf away from door

					}
				}
				
				this.state = SantaState.SLEEPING; // Santa goes back to sleep
				break;
			case WOKEN_UP_BY_REINDEER:
				// FIXME: assemble the reindeer to the sleigh then change state to ready
				break;
			case READY_FOR_CHRISTMAS: // nothing more to be done
				break;
			}
		}
	}


	/**
	 * Report about my state
	 */
	public void report() {
		System.out.println("Santa : " + state);
	}

	// stop after 370 days
	public void deferredTermination()
	{
		this.running = false;
	}

	// woken by elves
	public void wokenUpByElf()
	{
		this.state = SantaState.WOKEN_UP_BY_ELVES;
	}

}
