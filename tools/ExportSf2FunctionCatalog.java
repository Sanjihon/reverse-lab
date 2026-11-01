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
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;

public class ExportSf2FunctionCatalog extends GhidraScript {
    private static String cell(String value) {
        return value.replace("|", "\\|").replace("\r", " ").replace("\n", " ");
    }

    private static String hex(Address address) {
        return String.format("0x%08x", address.getOffset());
    }

    @Override
    public void run() throws Exception {
        String languageId = currentProgram.getLanguage().getLanguageID().toString();
        long imageBase = currentProgram.getImageBase().getOffset();
        if (!"68000:BE:32:default".equals(languageId) || imageBase != 0) {
            throw new IllegalStateException("Unexpected project language or image base");
        }

        Path report = Paths.get("/home/user/reverse-lab/reports/COMPLETE_FUNCTION_CATALOG.md");
        if (Files.exists(report)) {
            throw new IllegalStateException("Refusing to overwrite existing report: " + report);
        }

        List<Function> functions = new ArrayList<>();
        FunctionIterator iterator = currentProgram.getFunctionManager().getFunctions(true);
        while (iterator.hasNext()) {
            functions.add(iterator.next());
        }

        Map<String, Integer> xrefTypes = new TreeMap<>();
        int xrefCount = 0;
        AddressIterator sources = currentProgram.getReferenceManager()
                .getReferenceSourceIterator(currentProgram.getMemory().getMinAddress(), true);
        while (sources.hasNext()) {
            Address source = sources.next();
            for (Reference reference : currentProgram.getReferenceManager().getReferencesFrom(source)) {
                String type = reference.getReferenceType().toString();
                xrefTypes.put(type, xrefTypes.getOrDefault(type, 0) + 1);
                xrefCount++;
            }
        }

        StringBuilder markdown = new StringBuilder();
        markdown.append("# Vollstaendiger Ghidra-Funktionskatalog\n\n");
        markdown.append("Projekt: `working/SF2MainCPU.gpr`; Image `working/maincpu.bin`; ")
                .append("Ghidra 12.1.4, Language `").append(languageId)
                .append("`, Image Base `0x00000000`.\n\n");
        markdown.append("Dieser Katalog wurde aus dem aktuellen gespeicherten Ghidra-Projekt ")
                .append("erzeugt. Endadressen sind inklusiv; Groesse ist Ghidras Anzahl ")
                .append("belegter Adressbytes im Funktionskoerper. Disjunkte oder durch ")
                .append("Analysefehler beeinflusste Funktionskoerper koennen eine groessere ")
                .append("Adressspanne als die Bytezahl haben. Namen und Symbolquellen sind ")
                .append("unveraendert aus Ghidra uebernommen; es wurden keine Funktionen ")
                .append("manuell umbenannt.\n\n");
        markdown.append("Funktionen: **").append(functions.size())
                .append("**. Alle Cross-References im Ghidra-ReferenceManager: **")
                .append(xrefCount).append("**. Vollstaendige Kanten mit Typ, Quelle, Ziel ")
                .append("und Operandindex: `working/final_reference_inventory.log`.\n\n");
        markdown.append("| Start | Ende inkl. | Bytes | Ghidra-Name | Symbolquelle | Caller | Calls | Computed Calls | Data-Ziele | Register gelesen | Register geschrieben |\n");
        markdown.append("|---:|---:|---:|---|---|---:|---:|---:|---:|---|---|\n");

        for (Function function : functions) {
            AddressSetView body = function.getBody();
            Set<String> readRegisters = new TreeSet<>();
            Set<String> writtenRegisters = new TreeSet<>();
            Set<String> dataTargets = new TreeSet<>();
            int calls = 0;
            int computedCalls = 0;
            InstructionIterator instructions = currentProgram.getListing()
                    .getInstructions(body, true);
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
                        calls++;
                    } else if (reference.getReferenceType().isComputed()
                            && instruction.getFlowType().isCall()) {
                        computedCalls++;
                    }
                    if (reference.getReferenceType().isData()) {
                        dataTargets.add(reference.getToAddress().toString());
                    }
                }
            }

            int callers = 0;
            ReferenceIterator incoming = currentProgram.getReferenceManager()
                    .getReferencesTo(function.getEntryPoint());
            while (incoming.hasNext()) {
                if (incoming.next().getReferenceType().isCall()) {
                    callers++;
                }
            }

            markdown.append("| `").append(hex(function.getEntryPoint())).append("` | `")
                    .append(hex(body.getMaxAddress())).append("` | ")
                    .append(body.getNumAddresses()).append(" | `")
                    .append(cell(function.getName())).append("` | `")
                    .append(function.getSymbol().getSource()).append("` | ")
                    .append(callers).append(" | ").append(calls).append(" | ")
                    .append(computedCalls).append(" | ").append(dataTargets.size())
                    .append(" | `").append(cell(String.join(",", readRegisters))).append("` | `")
                    .append(cell(String.join(",", writtenRegisters))).append("` |\n");
        }

        Files.writeString(report, markdown.toString(), StandardCharsets.UTF_8);
        println("CATALOG_PATH\t" + report);
        println("CATALOG_FUNCTIONS\t" + functions.size());
        println("CATALOG_XREFS\t" + xrefCount);
        for (Map.Entry<String, Integer> entry : xrefTypes.entrySet()) {
            println("CATALOG_XREF_TYPE\t" + entry.getKey() + "\t" + entry.getValue());
        }
    }
}
