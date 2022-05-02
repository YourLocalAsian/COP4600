import java.util.ArrayList;
import java.util.List;
import java.util.Queue;
import java.util.LinkedList;


public class SantaScenario {

	public Santa santa;
	public List<Elf> elves;
	public List<Reindeer> reindeers;
	public boolean isDecember;

	public List<Elf> elvesAtDoor;
	public Queue<Elf> waitingForDoor;

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
		// The reindeer: in this case: 0
		scenario.reindeers = new ArrayList<>();
		for(int i=0; i != 0; i++) {
			Reindeer reindeer = new Reindeer(i+1, scenario);
			scenario.reindeers.add(reindeer);
			th = new Thread(reindeer);
			th.start();
		}

		// create lists and queues
		scenario.elvesAtDoor = new ArrayList<Elf>();
		scenario.waitingForDoor = new LinkedList<Elf>();

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
		}
	}

	public void sendElvesToSanta()
	{
		for (int i = 0; i < 3; i++) // send first 3 elves to Santa
		{
			this.elvesAtDoor.add(this.waitingForDoor.poll());
		}
		for (Elf elf : this.elvesAtDoor) // actual set to door
			elf.setState(Elf.ElfState.AT_SANTAS_DOOR);
	}

}
