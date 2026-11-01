import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.CodeUnit;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.mem.MemoryBlock;
import java.util.Map;
import java.util.TreeMap;

public class InspectSf2InputLiterals extends GhidraScript {
    @Override
    public void run() throws Exception {
        String languageId = currentProgram.getLanguage().getLanguageID().toString();
        long imageBase = currentProgram.getImageBase().getOffset();
        if (!"68000:BE:32:default".equals(languageId) || imageBase != 0) {
            throw new IllegalStateException("Unexpected project language or image base");
        }

        MemoryBlock block = currentProgram.getMemory().getBlock("ram");
        if (block == null) {
            throw new IllegalStateException("Expected imported raw image block 'ram'");
        }
        byte[] bytes = new byte[(int) block.getSize()];
        currentProgram.getMemory().getBytes(block.getStart(), bytes);

        int rawHits = 0;
        int instructionHits = 0;
        int dataHits = 0;
        int undefinedHits = 0;
        Map<Address, Instruction> instructions = new TreeMap<>();
        for (int offset = 0; offset <= bytes.length - 4; offset++) {
            long value = ((long) (bytes[offset] & 0xff) << 24)
                    | ((long) (bytes[offset + 1] & 0xff) << 16)
                    | ((long) (bytes[offset + 2] & 0xff) << 8)
                    | (long) (bytes[offset + 3] & 0xff);
            if (!((value >= 0x800000 && value <= 0x800007)
                    || (value >= 0x800018 && value <= 0x80001f))) {
                continue;
            }

            rawHits++;
            Address hit = block.getStart().add(offset);
            CodeUnit unit = currentProgram.getListing().getCodeUnitContaining(hit);
            if (unit instanceof Instruction) {
                instructionHits++;
                Instruction instruction = (Instruction) unit;
                instructions.put(instruction.getAddress(), instruction);
                println("INSTRUCTION_LITERAL_HIT\t" + hit + "\t"
                        + instruction.getAddress() + "\t" + instruction);
            } else if (unit != null) {
                dataHits++;
                Data data = (Data) unit;
                println("DATA_LITERAL_HIT\t" + hit + "\t" + data.getAddress() + "\t"
                        + data.getLength() + "\t" + data.getDataType().getName() + "\t"
                        + String.valueOf(data.getValue()));
            } else {
                undefinedHits++;
            }
        }

        println("INPUT_PORT_LITERAL_SCAN");
        println("RAW_LONGWORD_HITS\t" + rawHits);
        println("INSTRUCTION_COVERED_HITS\t" + instructionHits);
        println("DATA_COVERED_HITS\t" + dataHits);
        println("UNDEFINED_HITS\t" + undefinedHits);
        for (Instruction instruction : instructions.values()) {
            Function function = currentProgram.getFunctionManager()
                    .getFunctionContaining(instruction.getAddress());
            println("INPUT_LITERAL_INSTRUCTION\t" + instruction.getAddress() + "\t"
                    + instruction + "\t"
                    + (function == null ? "NO_FUNCTION" : function.getName()));
        }
    }
}
