import type { Request, Response } from "express";
import type { FrameParserConfig } from "./frame-parser.types";
import * as frameParserService from "./frame-parser.service";

export async function getConfig(_req: Request, res: Response) {
  
  res.status(200).json();
}