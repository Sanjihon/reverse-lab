import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressRange;
import ghidra.program.model.address.AddressRangeIterator;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.SourceType;

public class ExportSf2SourceEvidence extends GhidraScript {
    private static String bytes(Instruction instruction) throws Exception {
        StringBuilder value = new StringBuilder();
        for (byte item : instruction.getBytes()) {
            value.append(String.format("%02x", item & 0xff));
        }
        return value.toString();
    }

    @Override
    public void run() throws Exception {
        String languageId = currentProgram.getLanguage().getLanguageID().toString();
        long imageBase = currentProgram.getImageBase().getOffset();
        if (!"68000:BE:32:default".equals(languageId) || imageBase != 0) {
            throw new IllegalStateException("Unexpected project language or image base");
        }
        long[] targets = {
            0x400, 0x402, 0x40c, 0x40e, 0x73c, 0x752,
            0x7d6e2, 0x7e25e, 0x7e306, 0x7e30a, 0x7e318, 0x7e326,
            0x7e34c, 0x7e35a, 0x7e380, 0x7e3a6, 0x7e420, 0x7e430,
            0x7e440, 0x7e450, 0x7e460, 0x7e486, 0x7e492, 0x7f1b4, 0x83588,
            0x83838, 0x839ea
        };
        println("SOURCE_EVIDENCE\t" + currentProgram.getName() + "\t" + languageId
                + "\tbase=0x" + Long.toHexString(imageBase));

        for (long offset : targets) {
            Address address = toAddr(offset);
            Function function = currentProgram.getFunctionManager().getFunctionAt(address);
            if (function == null) {
                function = currentProgram.getFunctionManager().getFunctionContaining(address);
            }
            println("TARGET\t" + address + "\t"
                    + (function == null ? "NO_FUNCTION" : function.getEntryPoint())
                    + "\t" + (function == null ? "" : function.getName())
                    + "\t" + (function == null ? "" : function.getSymbol().getSource())
                    + "\t" + (function == null ? "0" : function.getBody().getNumAddresses()));
            if (function == null) {
                continue;
            }

            AddressRangeIterator ranges = function.getBody().getAddressRanges();
            while (ranges.hasNext()) {
                AddressRange range = ranges.next();
                println("RANGE\t" + function.getEntryPoint() + "\t"
                        + range.getMinAddress() + "\t" + range.getMaxAddress());
            }

                int count = 0;
                int instructionLimit = function.getEntryPoint().getOffset() == 0x7e492 ? 96 : 48;
            InstructionIterator instructions = currentProgram.getListing()
                    .getInstructions(function.getBody(), true);
                while (instructions.hasNext() && count < instructionLimit) {
                Instruction instruction = instructions.next();
                println("ASM\t" + function.getEntryPoint() + "\t"
                        + instruction.getAddress() + "\t" + bytes(instruction)
                        + "\t" + instruction);
                for (Reference reference : currentProgram.getReferenceManager()
                        .getReferencesFrom(instruction.getAddress())) {
                    println("INSTR_XREF\t" + instruction.getAddress() + "\t"
                            + reference.getToAddress() + "\t"
                            + reference.getReferenceType() + "\t"
                            + reference.getOperandIndex());
                }
                count++;
            }
            if (instructions.hasNext()) {
                println("ASM_TRUNCATED\t" + function.getEntryPoint()
                        + "\tmax_instructions=" + instructionLimit);
            }
        }
    }
}
