import type { ApiMessage } from "../../common/types/api.types";

export async function readDbc(): Promise<ApiMessage> {
  return {msg: "Read DBC"};
}

export async function writeDbc( _newDbc: unknown,): Promise<ApiMessage> {
  return {msg: "Write DBC"};
}
