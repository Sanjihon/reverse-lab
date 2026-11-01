import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.Map;

public class DecompileSf2Candidates extends GhidraScript {
    @Override
    public void run() throws Exception {
        String languageId = currentProgram.getLanguage().getLanguageID().toString();
        long imageBase = currentProgram.getImageBase().getOffset();
        if (!"68000:BE:32:default".equals(languageId) || imageBase != 0) {
            throw new IllegalStateException("Unexpected project language or image base");
        }

        Map<Address, Function> candidates = new LinkedHashMap<>();
        long[] vectorTargets = {
            0x400, 0x402, 0x40c, 0xa94, 0xb24, 0xb8c, 0xbe4, 0xcc4,
            0xcf0, 0xd18, 0xd34, 0xb62, 0xc12, 0xbb0, 0xb46, 0xc60, 0xca2, 0xca6
        };

        for (long offset : vectorTargets) {
            Address target = toAddr(offset);
            Function function = currentProgram.getFunctionManager().getFunctionContaining(target);
            if (function == null) {
                disassemble(target);
                function = getFunctionAt(target);
                if (function == null) {
                    function = createFunction(target, null);
                }
            }
            if (function != null) {
                candidates.put(function.getEntryPoint(), function);
                println("VECTOR_FUNCTION\t" + target + "\t" + function.getEntryPoint()
                        + "\t" + function.getName());
            } else {
                println("VECTOR_UNRESOLVED\t" + target);
            }
        }

        long[] functionCandidates = {
            0x40e, 0x1b1a, 0x2540, 0x83588, 0x7d6e2, 0x9314,
            0x839ea, 0x83838, 0x7d9b4, 0x7f1b4,
            0x7f56a, 0x7f62e, 0x7f8a4, 0x7f104, 0x7e3f6, 0x7e25e, 0x7dcd6,
            0x7e420, 0x7e430, 0x7e440, 0x7e450, 0x7e486, 0x7e492,
            0x7e306, 0x7e30a, 0x7e318, 0x7e326, 0x7e34c, 0x7e35a, 0x7e380, 0x7e3a6,
            0x84192, 0x84210, 0x84288, 0x844a2, 0x84536, 0x84734, 0x8487e,
            0x850ca, 0x850ec, 0x8532c, 0x85346, 0x85390, 0x853a0, 0x853b8,
            0x853d4, 0x853f4, 0x85408, 0x85418,
            0x9476, 0x8338, 0xcb8, 0x869ae, 0x21e2, 0x50ce
        };
        long[] collisionDispatchTargets = {
            0x7e306, 0x7e30a, 0x7e318, 0x7e326,
            0x7e34c, 0x7e35a, 0x7e380, 0x7e3a6
        };
        for (long offset : collisionDispatchTargets) {
            Address address = toAddr(offset);
            if (getFunctionAt(address) == null) {
                disassemble(address);
                if (getFunctionAt(address) == null) {
                    createFunction(address, null);
                }
            }
            Function function = getFunctionAt(address);
            if (function != null) {
                candidates.put(function.getEntryPoint(), function);
                println("COLLISION_DISPATCH_TARGET\t" + address + "\t"
                        + function.getName());
            } else {
                println("COLLISION_TARGET_UNRESOLVED\t" + address);
            }
        }

        for (long offset : functionCandidates) {
            Address address = toAddr(offset);
            Function function = getFunctionAt(address);
            if (function == null) {
                function = currentProgram.getFunctionManager().getFunctionContaining(address);
            }
            if (function != null) {
                candidates.put(function.getEntryPoint(), function);
            } else {
                println("CANDIDATE_WITHOUT_FUNCTION\t" + address);
            }
        }

        DecompInterface decompiler = new DecompInterface();
        boolean decompilerOpened = decompiler.openProgram(currentProgram);
        println("DECOMPILER_OPEN\t" + decompilerOpened + "\t" + decompiler.getLastMessage());
        for (Function function : new ArrayList<>(candidates.values())) {
            println("FUNCTION\t" + function.getEntryPoint() + "\t"
                    + function.getBody().getMaxAddress() + "\t"
                    + function.getBody().getNumAddresses() + "\t" + function.getName());
            DecompileResults result = decompiler.decompileFunction(function, 30, monitor);
            if (!result.decompileCompleted() || result.getDecompiledFunction() == null) {
                println("DECOMPILER_FAILURE\t" + function.getEntryPoint() + "\t"
                        + result.getErrorMessage());
                continue;
            }

            String code = result.getDecompiledFunction().getC();
                println("DECOMPILER_STATUS\t" + function.getEntryPoint() + "\tcompleted="
                    + result.decompileCompleted() + "\terror=" + result.getErrorMessage()
                    + "\tcLength=" + code.length() + "\tsignature="
                    + result.getDecompiledFunction().getSignature());
            if (code.length() > 6000) {
                code = code.substring(0, 6000) + "\n/* truncated by analysis script */";
            }
                String escapedCode = code.replace("\\", "\\\\")
                    .replace("\r", "\\r").replace("\n", "\\n");
                println("DECOMPILED_C\t" + function.getEntryPoint() + "\t" + escapedCode);
        }
        decompiler.dispose();
    }
}