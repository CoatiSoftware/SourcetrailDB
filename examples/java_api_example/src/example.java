import com.sourcetrail.*;

public class example {
  private static int DBVersion;
  private static String DBFilePath;
  private static String SourceFilePath;

    public static void main(String[] args) {

      for (String arg : args) {
        String name = "";
        String value = "";
        try {
          name = arg.split("=")[0];
          value = arg.split("=")[1];
        } catch(Exception e) {
          System.out.println("Invalid parameter:" + arg);
          System.exit(1);
        }
        switch(name) {
          case "--database-file-path":
            example.DBFilePath = value;
            break;

          case "--source-file-path":
            example.SourceFilePath = value;
            break;

          case "--database-version":
            try {
              example.DBVersion = Integer.parseInt(value);
            } catch (Exception e) {
              System.out.println("Invalid DB version:" + value);
            }
            break;

          default:
            System.out.println("Invalid parameter:" + name);
            System.exit(1);
        }
      }

      System.out.println("SourcetrailDB Java API Example");
      System.out.println("Supported database version: " + sourcetraildb.getSupportedDatabaseVersion());

      if (example.DBVersion > 0 && example.DBVersion != sourcetraildb.getSupportedDatabaseVersion()) {
        System.out.println("ERROR: Only supports database version: " + sourcetraildb.getSupportedDatabaseVersion() +
        ". Requested version: " + example.DBVersion);
        System.exit(1);
      }

      try {
        sourcetraildb.open(example.DBFilePath);
      } catch (Exception e) {
        System.out.println(e.getMessage());
        System.exit(1);
      }

      System.out.println("Clearing loaded database now...");
      sourcetraildb.clear();
      
      System.out.println("start indexing");
      sourcetraildb.beginTransaction();
    
      int fileId = sourcetraildb.recordFile(example.SourceFilePath);
      sourcetraildb.recordFileLanguage(fileId, "python");
      
      if (sourcetraildb.getLastError().length() > 0) {
        System.out.println(sourcetraildb.getLastError());
        System.exit(1);
      }

      int symbolId = sourcetraildb.recordSymbol(
        "{ \"name_delimiter\": \".\", \"name_elements\": [ " +
          "{ \"prefix\": \"\", \"name\": \"MyType\", \"postfix\": \"\" } " +
        "] }");
      sourcetraildb.recordSymbolDefinitionKind(symbolId, DefinitionKind.DEFINITION_EXPLICIT);
      sourcetraildb.recordSymbolKind(symbolId, SymbolKind.SYMBOL_CLASS);
      sourcetraildb.recordSymbolLocation(symbolId, fileId, 2, 7, 2, 12);
      sourcetraildb.recordSymbolScopeLocation(symbolId, fileId, 2, 1, 7, 1);
      
      int memberId = sourcetraildb.recordSymbol(
        "{ \"name_delimiter\": \".\", \"name_elements\": [ " +
          "{ \"prefix\": \"\", \"name\": \"MyType\", \"postfix\": \"\" }, " +
          "{ \"prefix\": \"\", \"name\": \"my_member\", \"postfix\": \"\" } " +
        "] }");
      sourcetraildb.recordSymbolDefinitionKind(memberId, DefinitionKind.DEFINITION_EXPLICIT);
      sourcetraildb.recordSymbolKind(memberId, SymbolKind.SYMBOL_FIELD);
      sourcetraildb.recordSymbolLocation(memberId, fileId, 4, 2, 4, 10);

      int methodId = sourcetraildb.recordSymbol(
        "{ \"name_delimiter\": \".\", \"name_elements\": [ " +
          "{ \"prefix\": \"\", \"name\": \"MyType\", \"postfix\": \"\" }, " +
          "{ \"prefix\": \"\", \"name\": \"my_method\", \"postfix\": \"\" } " +
        "] }");
      sourcetraildb.recordSymbolDefinitionKind(methodId, DefinitionKind.DEFINITION_EXPLICIT);
      sourcetraildb.recordSymbolKind(methodId, SymbolKind.SYMBOL_METHOD);
      sourcetraildb.recordSymbolLocation(methodId, fileId, 6, 6, 6, 14);
      sourcetraildb.recordSymbolScopeLocation(methodId, fileId, 6, 1, 7, 1);

      int useageId = sourcetraildb.recordReference(methodId, memberId, ReferenceKind.REFERENCE_USAGE);
      sourcetraildb.recordReferenceLocation(useageId, fileId, 7, 10, 7, 18);

      sourcetraildb.commitTransaction();
      
      if (sourcetraildb.getLastError().length() > 0) {
        System.out.println(sourcetraildb.getLastError());
        System.exit(1);
      }

      if (!sourcetraildb.close()) {
        System.out.println(sourcetraildb.getLastError());
        System.exit(1);
      }

      System.out.println("done");
    }
}
