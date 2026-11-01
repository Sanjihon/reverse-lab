import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.DataIterator;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;
import java.util.Locale;

public class InspectSf2UiStrings extends GhidraScript {
    @Override
    public void run() throws Exception {
        String languageId = currentProgram.getLanguage().getLanguageID().toString();
        long imageBase = currentProgram.getImageBase().getOffset();
        if (!"68000:BE:32:default".equals(languageId) || imageBase != 0) {
            throw new IllegalStateException("Unexpected project language or image base");
        }

        int matches = 0;
        int referenced = 0;
        DataIterator dataIterator = currentProgram.getListing().getDefinedData(true);
        while (dataIterator.hasNext()) {
            Data data = dataIterator.next();
            Object value = data.getValue();
            if (!(value instanceof String)) {
                continue;
            }
            String text = (String) value;
            String normalized = text.toUpperCase(Locale.ROOT);
        if (!(normalized.contains("START") || normalized.contains("SELECT")
            || normalized.contains("ROUND") || normalized.contains("FIGHT")
            || normalized.contains("PLAYER") || normalized.contains("WIN")
            || normalized.contains("LOSE") || normalized.contains("CONTINUE")
            || normalized.contains("CREDIT") || normalized.contains("TITLE")
            || normalized.contains("STAGE") || normalized.contains("COIN"))) {
        continue;
        }

        matches++;
        String type = data.getDataType().getName();
        println("UI_STRING\t" + data.getAddress() + "\t" + data.getLength() + "\t"
            + type + "\t" + text.replace("\n", "\\n"));

        ReferenceIterator references = currentProgram.getReferenceManager()
            .getReferencesTo(data.getAddress());
        while (references.hasNext()) {
        Reference reference = references.next();
        Function function = currentProgram.getFunctionManager()
            .getFunctionContaining(reference.getFromAddress());
        String functionName = function == null ? "NO_FUNCTION" : function.getName();
        println("UI_STRING_REF\t" + data.getAddress() + "\t"
            + reference.getFromAddress() + "\t" + reference.getReferenceType()
            + "\t" + functionName);
        referenced++;
        }
    }
    println("UI_STRING_TOTAL\t" + matches);
    println("UI_STRING_REFERENCE_TOTAL\t" + referenced);
    }
}
