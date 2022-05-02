import java.util.ArrayList;
import java.util.List;
import java.util.Queue;
import java.util.LinkedList;
import java.util.concurrent.Semaphore;


public class SantaScenario {

	public Santa santa;
	public List<Elf> elves;
	public List<Reindeer> reindeers;
	public boolean isDecember;

	public List<Elf> elvesAtDoor;
	public Queue<Elf> waitingForDoor;
	public Semaphore elfInTrouble;
	public Semaphore elfSlot;
	public Semaphore doorReady;
	public Semaphore readyRudolph;
	public Semaphore enteringShed;
	public int reindeerInShed;

	public static void main(String args[]) {
		SantaScenario scenario = new SantaScenario();
		scenario.isDecember = false;
		// create the participants
		// Santa
		scenario.santa = new Santa(scenario);
		Thread th = new Thread(scenario.santa);
		th.start();
		// The elves: in this case: 10
		scenario.elves = new ArrayList<>();
		for(int i = 0; i != 10; i++) {
			Elf elf = new Elf(i+1, scenario);
			scenario.elves.add(elf);
			th = new Thread(elf);
			th.start();
		}
		// The reindeer: in this case: 9
		scenario.reindeers = new ArrayList<>();
		for(int i=0; i != 9; i++) {
			Reindeer reindeer = new Reindeer(i+1, scenario);
			scenario.reindeers.add(reindeer);
			th = new Thread(reindeer);
			th.start();
		}

		// create lists and queues
		scenario.elvesAtDoor = new ArrayList<Elf>();
		scenario.waitingForDoor = new LinkedList<Elf>();

		// create Semaphore
		scenario.elfInTrouble = new Semaphore(1);
		scenario.elfSlot = new Semaphore(3);
		scenario.doorReady = new Semaphore(0);
		scenario.readyRudolph = new Semaphore(0);
		scenario.enteringShed = new Semaphore(1);
		scenario.reindeerInShed = 0;

		// now, start the passing of time
		for(int day = 1; day < 500; day++) {
			// wait a day
			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			// turn on December
			if (day > (365 - 31)) {
				scenario.isDecember = true;
			}

			// deferred termination on day 370
			if (day == 370)
			{
				scenario.santa.deferredTermination(); // terminate Santa

				for(Elf elf: scenario.elves) // terminate all the elves
					elf.deferredTermination();

				for(Reindeer reindeer: scenario.reindeers) // terminate all the reindeers
					reindeer.deferredTermination();
			}

			// Check if 3 elves are waiting
			if (scenario.waitingForDoor.size() >= 3)
				scenario.sendElvesToSanta();

			// print out the state:
			System.out.println("***********  Day " + day + " *************************");
			scenario.santa.report();
			for(Elf elf: scenario.elves) {
				elf.report();
			}
			for(Reindeer reindeer: scenario.reindeers) {
				reindeer.report();
			}

			//System.out.println("Elves waiting : " + scenario.waitingForDoor.size());
			//System.out.println("Elf SP : " + scenario.elvesInTrouble.availablePermits());
			//System.out.println("Door : " + scenario.doorReady.availablePermits());
			System.out.println("RiS : " + scenario.reindeerInShed);
		}
	}

	public void sendElvesToSanta()
	{
		for (int i = 0; i < 3; i++) // send first 3 elves to Santa
		{
			this.elvesAtDoor.add(this.waitingForDoor.poll());
			this.doorReady.release(); // tell elves they're at the door
		}

		for (Elf elf : this.elvesAtDoor)
			elf.setState(Elf.ElfState.AT_SANTAS_DOOR);
	}

}
