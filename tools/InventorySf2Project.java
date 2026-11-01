import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressIterator;
import ghidra.program.model.address.AddressSetView;
import ghidra.program.model.lang.Register;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;
import ghidra.program.model.symbol.SourceType;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;

public class InventorySf2Project extends GhidraScript {
    @Override
    public void run() throws Exception {
        String languageId = currentProgram.getLanguage().getLanguageID().toString();
        long imageBase = currentProgram.getImageBase().getOffset();
        if (!"68000:BE:32:default".equals(languageId) || imageBase != 0) {
            throw new IllegalStateException("Unexpected project language or image base");
        }

        List<Function> functions = new ArrayList<>();
        Map<Address, Function> byEntry = new HashMap<>();
        FunctionIterator iterator = currentProgram.getFunctionManager().getFunctions(true);
        while (iterator.hasNext()) {
            Function function = iterator.next();
            functions.add(function);
            byEntry.put(function.getEntryPoint(), function);
        }

        println("PROJECT\t" + currentProgram.getName());
        println("LANGUAGE\t" + languageId);
        println(String.format("IMAGE_BASE\t0x%08x", imageBase));
        println("FUNCTION_COUNT\t" + functions.size());
        println("FUNCTIONS\tstart\tend_inclusive\tbytes\tname\tsymbol_source\tincoming_calls\tdirect_calls\tcomputed_calls\tdata_refs\tregisters_read\tregisters_written");

        for (Function function : functions) {
            AddressSetView body = function.getBody();
            Set<String> readRegisters = new TreeSet<>();
            Set<String> writtenRegisters = new TreeSet<>();
            Set<String> callees = new TreeSet<>();
            Set<String> dataTargets = new TreeSet<>();
            int directCalls = 0;
            int computedCalls = 0;

            InstructionIterator instructions = currentProgram.getListing().getInstructions(body, true);
            while (instructions.hasNext()) {
                Instruction instruction = instructions.next();
                for (Object input : instruction.getInputObjects()) {
                    if (input instanceof Register) {
                        readRegisters.add(((Register) input).getName());
                    }
                }
                for (Object result : instruction.getResultObjects()) {
                    if (result instanceof Register) {
                        writtenRegisters.add(((Register) result).getName());
                    }
                }

                for (Reference reference : currentProgram.getReferenceManager()
                        .getReferencesFrom(instruction.getAddress())) {
                    if (reference.getReferenceType().isCall()) {
                        directCalls++;
                        Function callee = currentProgram.getFunctionManager()
                                .getFunctionAt(reference.getToAddress());
                        if (callee == null) {
                            callee = currentProgram.getFunctionManager()
                                    .getFunctionContaining(reference.getToAddress());
                        }
                        String calleeName = callee == null ? "UNRESOLVED" : callee.getName();
                        callees.add(reference.getToAddress() + ":" + calleeName);
                        println("CALL\t" + function.getEntryPoint() + "\t"
                                + instruction.getAddress() + "\t" + reference.getToAddress()
                                + "\t" + calleeName);
                    } else if (reference.getReferenceType().isComputed()
                            && instruction.getFlowType().isCall()) {
                        computedCalls++;
                        println("INDIRECT_CALL\t" + function.getEntryPoint() + "\t"
                                + instruction.getAddress() + "\t" + instruction);
                    }

                    if (reference.getReferenceType().isData()) {
                        dataTargets.add(reference.getToAddress().toString());
                        println("DATA_REF\t" + function.getEntryPoint() + "\t"
                                + instruction.getAddress() + "\t" + reference.getToAddress());
                    }
                }
            }

            int incomingCalls = 0;
            ReferenceIterator incoming = currentProgram.getReferenceManager()
                    .getReferencesTo(function.getEntryPoint());
            while (incoming.hasNext()) {
                if (incoming.next().getReferenceType().isCall()) {
                    incomingCalls++;
                }
            }

            SourceType source = function.getSymbol().getSource();
            println("FUNCTION\t" + function.getEntryPoint() + "\t"
                    + function.getBody().getMaxAddress() + "\t"
                    + function.getBody().getNumAddresses() + "\t"
                    + function.getName() + "\t" + source + "\t"
                    + incomingCalls + "\t" + directCalls + "\t" + computedCalls + "\t"
                    + dataTargets.size() + "\t" + String.join(",", readRegisters) + "\t"
                    + String.join(",", writtenRegisters));
        }

        println("VECTOR_TARGETS");
        long[] vectorOffsets = {
            0x400, 0x402, 0x40c, 0xa94, 0xb24, 0xb8c, 0xbe4, 0xcc4,
            0xcf0, 0xd18, 0xd34, 0xb62, 0xc12, 0xbb0, 0xb46, 0xc60, 0xca2, 0xca6
        };
        Set<Long> uniqueTargets = new HashSet<>();
        for (long offset : vectorOffsets) {
            if (!uniqueTargets.add(offset)) {
                continue;
            }
            Address target = toAddr(offset);
            Function function = currentProgram.getFunctionManager().getFunctionAt(target);
            if (function == null) {
                function = currentProgram.getFunctionManager().getFunctionContaining(target);
            }
            println("VECTOR_TARGET\t" + target + "\t"
                    + (function == null ? "NO_FUNCTION" : function.getEntryPoint() + ":" + function.getName()));
        }

        int xrefCount = 0;
        Map<String, Integer> xrefTypes = new TreeMap<>();
        AddressIterator referenceSources = currentProgram.getReferenceManager()
                .getReferenceSourceIterator(currentProgram.getMemory().getMinAddress(), true);
        while (referenceSources.hasNext()) {
            Address source = referenceSources.next();
            for (Reference reference : currentProgram.getReferenceManager().getReferencesFrom(source)) {
                String type = reference.getReferenceType().toString();
                xrefCount++;
                xrefTypes.put(type, xrefTypes.getOrDefault(type, 0) + 1);
                println("XREF\t" + reference.getFromAddress() + "\t"
                        + reference.getToAddress() + "\t" + type + "\t"
                        + reference.getOperandIndex());
            }
        }
        println("XREF_TOTAL\t" + xrefCount);
        for (Map.Entry<String, Integer> entry : xrefTypes.entrySet()) {
            println("XREF_TYPE\t" + entry.getKey() + "\t" + entry.getValue());
        }
    }
}