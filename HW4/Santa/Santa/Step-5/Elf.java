import java.util.Random;

public class Elf implements Runnable {

	enum ElfState {
		WORKING, TROUBLE, AT_SANTAS_DOOR
	};

	private ElfState state;
	/**
	 * The number associated with the Elf
	 */
	private int number;
	private Random rand = new Random();
	private SantaScenario scenario;
	private boolean running;
	private boolean waiting;

	public Elf(int number, SantaScenario scenario) {
		this.number = number;
		this.scenario = scenario;
		this.state = ElfState.WORKING;
		this.running = true;
		this.waiting = false;
	}


	public ElfState getState() {
		return state;
	}

	/**
	 * Santa might call this function to fix the trouble
	 * @param state
	 */
	public void setState(ElfState state) {
		this.state = state;
	}


	@Override
	public void run() {
		while (this.running) {
      // wait a day
  		try {
  			Thread.sleep(100);
  		} catch (InterruptedException e) {
  			// TODO Auto-generated catch block
  			e.printStackTrace();
  		}
			switch (state) {
			case WORKING: {
				// at each day, there is a 1% chance that an elf runs into
				// trouble.
				if (rand.nextDouble() < 0.01) {
					state = ElfState.TROUBLE;
				}
				break;
			}
			case TROUBLE: {
				// FIXME: if possible, move to Santa's door
				try
				{
					if(!this.waiting)
					{
						scenario.elfSlot.acquire();

						scenario.elfInTrouble.acquire();
						scenario.waitingForDoor.add(this); // add to waiting queue
						scenario.elfInTrouble.release();
						this.waiting = true;
					}

					scenario.doorReady.acquire(); // wait for three elves to be ready
				}
				catch(Exception e)
				{
					e.printStackTrace();
				}

				break;
			}
			case AT_SANTAS_DOOR:
				// FIXME: if feasible, wake up Santa
				scenario.santa.wokenUpByElf();
				this.waiting = false;
				break;
			}
		}
	}

	/**
	 * Report about my state
	 */
	public void report() {
		System.out.println("Elf " + number + " : " + state);
	}

	// stop after 370 days
	public void deferredTermination()
	{
		this.running = false;
	}

}
