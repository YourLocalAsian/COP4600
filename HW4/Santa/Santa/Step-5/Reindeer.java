import java.util.Random;


public class Reindeer implements Runnable {

	public enum ReindeerState {AT_BEACH, AT_WARMING_SHED, AT_THE_SLEIGH};
	private ReindeerState state;
	private SantaScenario scenario;
	private Random rand = new Random();
	private boolean running;

	/**
	 * The number associated with the reindeer
	 */
	private int number;

	/**
	 * Santa might call this function to fix the trouble
	 * @param state
	 */
	public void setState(ReindeerState state) {
		this.state = state;
	}

	public Reindeer(int number, SantaScenario scenario) {
		this.number = number;
		this.scenario = scenario;
		this.state = ReindeerState.AT_BEACH;
		this.running = true;
	}

	@Override
	public void run() {
		while(this.running) {
		// wait a day
		try {
			Thread.sleep(100);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		// see what we need to do:
		switch(state) {
		case AT_BEACH: { // if it is December, the reindeer might think about returning from the beach
			if (scenario.isDecember) {
				if (rand.nextDouble() < 0.1) {
					state = ReindeerState.AT_WARMING_SHED;
				}
			}
			break;
		}
		case AT_WARMING_SHED:
			// if all the reindeer are home, wake up santa
			if (scenario.reindeerInShed < 8)
			{
				try
				{
					scenario.enteringShed.acquire();
					scenario.reindeerInShed++; // increment reindeers
					scenario.enteringShed.release();

					scenario.readyRudolph.acquire(); // wait for all the reindeer
				}
				catch (Exception e)
				{
					e.printStackTrace();
				}
			}
			else
			{
				//System.out.println("Waking him up");
				scenario.santa.wokenUpByReindeer();
			}

			break;
		case AT_THE_SLEIGH:
			// keep pulling
			break;
		}
		}
	};

	/**
	 * Report about my state
	 */
	public void report() {
		System.out.println("Reindeer " + number + " : " + state);
	}

	// stop after 370 days
	public void deferredTermination()
	{
		this.running = false;
	}
}
