import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressRange;
import ghidra.program.model.address.AddressRangeIterator;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;
import java.util.LinkedHashMap;
import java.util.Map;

public class InspectSf2Phase10 extends GhidraScript {
    private static String hex(Address address) {
        return String.format("0x%08x", address.getOffset());
    }

    private static String escaped(String value) {
        return value.replace("\\", "\\\\").replace("\r", "\\r")
                .replace("\n", "\\n").replace("\t", "\\t");
    }

    @Override
    public void run() throws Exception {
        String languageId = currentProgram.getLanguage().getLanguageID().toString();
        long imageBase = currentProgram.getImageBase().getOffset();
        if (!"68000:BE:32:default".equals(languageId) || imageBase != 0) {
            throw new IllegalStateException("Unexpected project language or image base");
        }

        println("PROJECT\t" + currentProgram.getName() + "\t" + languageId
                + "\tbase=" + hex(currentProgram.getImageBase()));

        long[] instructionOffsets = {
            0x41e, 0x422, 0x426, 0x428, 0x42a,
            0x73c, 0x742, 0x748, 0x74c, 0x74e, 0x750
        };
        for (long offset : instructionOffsets) {
            Address address = toAddr(offset);
            Instruction instruction = currentProgram.getListing().getInstructionAt(address);
            println("INSTRUCTION\t" + hex(address) + "\t"
                    + (instruction == null ? "NOT_DECODED" : escaped(instruction.toString())));
            for (Reference reference : currentProgram.getReferenceManager()
                    .getReferencesFrom(address)) {
                println("INSTRUCTION_XREF\t" + hex(address) + "\t"
                        + hex(reference.getToAddress()) + "\t"
                        + reference.getReferenceType() + "\t" + reference.getOperandIndex());
            }
        }

        long[][] ioRanges = {{0x800000, 0x800007}, {0x800018, 0x80001f}};
        for (long[] range : ioRanges) {
            for (long offset = range[0]; offset <= range[1]; offset++) {
                Address address = toAddr(offset);
                ReferenceIterator references = currentProgram.getReferenceManager()
                        .getReferencesTo(address);
                while (references.hasNext()) {
                    Reference reference = references.next();
                    println("IO_XREF\t" + hex(reference.getFromAddress()) + "\t"
                            + hex(address) + "\t" + reference.getReferenceType());
                }
            }
        }

        long[] functionOffsets = {
            0x40e, 0x7d6e2, 0x7d9b4, 0x7f1b4, 0x7e25e,
            0x7e420, 0x7e430, 0x7e440, 0x7e450, 0x7e486, 0x7e492,
            0x83588
        };
        Map<Address, Function> functions = new LinkedHashMap<>();
        for (long offset : functionOffsets) {
            Address address = toAddr(offset);
            Function function = currentProgram.getFunctionManager().getFunctionAt(address);
            if (function == null) {
                function = currentProgram.getFunctionManager().getFunctionContaining(address);
            }
            println("TARGET\t" + hex(address) + "\t"
                    + (function == null ? "NO_FUNCTION" : hex(function.getEntryPoint())
                    + "\t" + function.getName()));
            if (function != null) {
                functions.put(function.getEntryPoint(), function);
            }
        }

        for (Function function : functions.values()) {
            println("FUNCTION\t" + hex(function.getEntryPoint()) + "\t"
                    + hex(function.getBody().getMaxAddress()) + "\t"
                    + function.getBody().getNumAddresses() + "\t" + function.getName());
            AddressRangeIterator ranges = function.getBody().getAddressRanges();
            while (ranges.hasNext()) {
                AddressRange range = ranges.next();
                println("RANGE\t" + hex(function.getEntryPoint()) + "\t"
                        + hex(range.getMinAddress()) + "\t" + hex(range.getMaxAddress()));
            }

            InstructionIterator instructions = currentProgram.getListing()
                    .getInstructions(function.getBody(), true);
            while (instructions.hasNext()) {
                Instruction instruction = instructions.next();
                for (Reference reference : currentProgram.getReferenceManager()
                        .getReferencesFrom(instruction.getAddress())) {
                    println("FUNCTION_XREF\t" + hex(function.getEntryPoint()) + "\t"
                            + hex(reference.getFromAddress()) + "\t"
                            + hex(reference.getToAddress()) + "\t"
                            + reference.getReferenceType() + "\t" + reference.getOperandIndex());
                }
            }

            ReferenceIterator incoming = currentProgram.getReferenceManager()
                    .getReferencesTo(function.getEntryPoint());
            while (incoming.hasNext()) {
                Reference reference = incoming.next();
                println("INCOMING_XREF\t" + hex(function.getEntryPoint()) + "\t"
                        + hex(reference.getFromAddress()) + "\t"
                        + reference.getReferenceType());
            }
        }

        DecompInterface decompiler = new DecompInterface();
        boolean opened = decompiler.openProgram(currentProgram);
        println("DECOMPILER_OPEN\t" + opened + "\t" + escaped(decompiler.getLastMessage()));
        long[] decompileOffsets = {0x40e, 0x7d6e2, 0x7d9b4, 0x7f1b4, 0x7e25e, 0x7e486, 0x7e492};
        for (long offset : decompileOffsets) {
            Function function = currentProgram.getFunctionManager().getFunctionAt(toAddr(offset));
            if (function == null) {
                println("DECOMPILER_NO_FUNCTION\t" + hex(toAddr(offset)));
                continue;
            }
            DecompileResults result = decompiler.decompileFunction(function, 30, monitor);
            if (!result.decompileCompleted() || result.getDecompiledFunction() == null) {
                println("DECOMPILER_FAILURE\t" + hex(function.getEntryPoint()) + "\t"
                        + escaped(result.getErrorMessage()));
                continue;
            }
            println("DECOMPILER_STATUS\t" + hex(function.getEntryPoint()) + "\tcompleted=true\t"
                    + escaped(result.getDecompiledFunction().getSignature()));
            println("DECOMPILED_C\t" + hex(function.getEntryPoint()) + "\t"
                    + escaped(result.getDecompiledFunction().getC()));
        }
        decompiler.dispose();
    }
}