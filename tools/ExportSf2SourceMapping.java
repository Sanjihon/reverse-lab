import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class ExportSf2SourceMapping extends GhidraScript {
    private static String csv(String value) {
        return "\"" + value.replace("\"", "\"\"") + "\"";
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

        Path report = Paths.get("/home/user/reverse-lab/reports/source_function_mapping.csv");
        if (Files.exists(report)) {
            throw new IllegalStateException("Refusing to overwrite existing mapping: " + report);
        }

        StringBuilder output = new StringBuilder();
        output.append("ghidra_start,ghidra_end_inclusive,ghidra_body_bytes,ghidra_name,")
                .append("symbol_source,reconstruction_status,c_function,c_file,mapping_scope\n");
        int confirmed = 0;
        int probable = 0;
        int unknown = 0;

        FunctionIterator functions = currentProgram.getFunctionManager().getFunctions(true);
        while (functions.hasNext()) {
            Function function = functions.next();
            long entry = function.getEntryPoint().getOffset();
            String status = "UNKNOWN";
            String cFunction = "";
            String cFile = "";
            String scope = "No reconstructed C implementation; function behavior not established in phase 9.";

            if (entry == 0x40e) {
                status = "CONFIRMED";
                cFunction = "recon_FUN_0000040e_prefix";
                cFile = "src/system/startup.c";
                scope = "Partial side-effect reconstruction of Ghidra body ranges 0x0000040e-0x00000425 and 0x0000073c-0x00000751; LEA xref resolves A4 to 0x00000426, but the JMP (A4) control transfer remains unmodeled.";
                confirmed++;
            } else if (entry == 0x7e486 || entry == 0x7e492) {
                status = "PROBABLE";
                cFunction = "recon_FUN_0007e486_overlap_model";
                cFile = "src/game/collision.c";
                scope = entry == 0x7e486
                        ? "Combined host-side model includes this 12-byte entry fragment and shared-body path; assembler branches into 0x0007e492."
                        : "Combined host-side model includes this 156-byte comparison body; modeled signature/ABI is inferred, not recovered.";
                probable++;
            } else {
                unknown++;
            }

            output.append(csv(hex(function.getEntryPoint()))).append(',')
                    .append(csv(hex(function.getBody().getMaxAddress()))).append(',')
                    .append(function.getBody().getNumAddresses()).append(',')
                    .append(csv(function.getName())).append(',')
                    .append(csv(function.getSymbol().getSource().toString())).append(',')
                    .append(status).append(',')
                    .append(csv(cFunction)).append(',')
                    .append(csv(cFile)).append(',')
                    .append(csv(scope)).append('\n');
        }

        Files.writeString(report, output.toString(), StandardCharsets.UTF_8);
        println("SOURCE_MAPPING_PATH\t" + report);
        println("SOURCE_MAPPING_FUNCTIONS\t" + (confirmed + probable + unknown));
        println("SOURCE_MAPPING_CONFIRMED\t" + confirmed);
        println("SOURCE_MAPPING_PROBABLE\t" + probable);
        println("SOURCE_MAPPING_UNKNOWN\t" + unknown);
    }
}
