import type { ApiMessage } from "../../common/types/api.types";
import type { ConfigJSON } from "./config.types";

export async function readConfig(): Promise<ConfigJSON | null> {
  // Read and return current config from shared memory
  
  return null;
}

export async function writeConfig( _newConfig: unknown,): Promise<ApiMessage> {
  // Write new config to shared memory

  // Notify graphic renderer of change to config
  
  return {msg: "Write Config"};
}

export async function resetConfig(): Promise<ApiMessage> {
  // Reset shared memory to base config 

  return {msg: "Reset Config"};
}
