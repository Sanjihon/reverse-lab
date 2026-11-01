import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Instruction;

public class InspectSf2Startup extends GhidraScript {
    @Override
    public void run() throws Exception {
        String languageId = currentProgram.getLanguage().getLanguageID().toString();
        long imageBase = currentProgram.getImageBase().getOffset();
        String compilerSpec = currentProgram.getCompilerSpec().getCompilerSpecID().toString();

        println("Ghidra language: " + languageId);
        println("Compiler spec: " + compilerSpec);
        println(String.format("Image base: 0x%08x", imageBase));

        if (!"68000:BE:32:default".equals(languageId)) {
            throw new IllegalStateException("Unexpected Ghidra language: " + languageId);
        }
        if (imageBase != 0) {
            throw new IllegalStateException(String.format("Unexpected image base: 0x%x", imageBase));
        }

        Address resetPc = toAddr(0x40e);
        disassemble(resetPc);
        if (getFunctionAt(resetPc) == null) {
            createFunction(resetPc, null);
        }

        dumpInstructions(resetPc, 12);
        long[] relatedTargets = {
            0x400, 0x402, 0x40c, 0x428, 0x73c, 0x752, 0x7ae, 0x7cc,
            0xa94, 0xb24, 0xb8c, 0xbe4, 0xcc4,
            0xcf0, 0xd18, 0xd34, 0xb62, 0xc12, 0xbb0, 0xb46, 0xc60, 0xca2, 0xca6
        };
        for (long target : relatedTargets) {
            dumpInstructions(toAddr(target), 8);
        }
    }

    private void dumpInstructions(Address start, int count) throws Exception {
        Address cursor = start;
        println("Instruction listing from " + start + ":");
        for (int index = 0; index < count; index++) {
            Instruction instruction = currentProgram.getListing().getInstructionAt(cursor);
            if (instruction == null) {
                disassemble(cursor);
                instruction = currentProgram.getListing().getInstructionAt(cursor);
            }
            if (instruction == null) {
                println(String.format("0x%08x: <not decoded>", cursor.getOffset()));
                break;
            }

            StringBuilder bytes = new StringBuilder();
            for (byte value : instruction.getBytes()) {
                bytes.append(String.format("%02x", value & 0xff));
            }
            StringBuilder operands = new StringBuilder();
            for (int operand = 0; operand < instruction.getNumOperands(); operand++) {
                if (operand > 0) {
                    operands.append(", ");
                }
                operands.append(instruction.getDefaultOperandRepresentation(operand));
            }
            println(String.format("0x%08x: %-12s %s %s", cursor.getOffset(), bytes,
                instruction.getMnemonicString(), operands));

            cursor = instruction.getMaxAddress().next();
        }
    }
}