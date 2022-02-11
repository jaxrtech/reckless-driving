import { Command } from 'commander';
import cheerio from 'cheerio';
import fetch from 'cross-fetch';

const groups = {
  'WindowManager': 'https://mirror.informatimago.com/next/developer.apple.com/documentation/Carbon/Reference/WindowManagerLegacy/wind_mgr_leg_ref/FunctionGroupIndex.html',
  'ResourceManager': 'https://mirror.informatimago.com/next/developer.apple.com/documentation/Carbon/Reference/Resource_Manager/resourcemgr/FunctionGroupIndex.html',
  'Quickdraw': 'https://mirror.informatimago.com/next/developer.apple.com/documentation/Carbon/Reference/QuickDraw_Ref/qdref_main/FunctionGroupIndex.html',
  'QuickdrawText': 'https://mirror.informatimago.com/next/developer.apple.com/documentation/Carbon/Reference/QuickDraw_Text/qdtext_refchap/FunctionGroupIndex.html',
  'DialogManager': 'https://mirror.informatimago.com/next/developer.apple.com/documentation/Carbon/Reference/Dialog_Manager/dialog_mgr_ref/FunctionGroupIndex.html',

  // TODO: Older Pascal documentation
  'EventManager': 'https://mirror.informatimago.com/next/developer.apple.com/documentation/mac/Toolbox/Toolbox-49.html',
  'Files': [
    'https://mirror.informatimago.com/next/developer.apple.com/documentation/mac/Files/Files-40.html',
    'https://mirror.informatimago.com/next/developer.apple.com/documentation/mac/Files/Files-51.html',
  ]
};

const program = new Command()
  .name('generate-stubs')
  .description("Generates C stubs from Apple's legacy HTML documention")
  .version('0.0.0');

program
  .command('ls')
  .description('list available groups')
  .action(() => {
    for (const [name, url] of Object.entries(groups)) {
      console.log(`${name}\t${url}`);
    }
  });

program
  .command('group <id>')
  .description('generates stubs for a group')
  .option('-o, --output <path>', 'output path to generated C file', '/dev/stdout')
  .action(function (this: Command, groupId) {
    const opts = this.opts();
    const outputPath = opts.output as string;
    console.error('TODO', { groupId, outputPath });
  });

program
  .command('url <url>')
  .description("generates stubs from a documentation's HTML page from a URL")
  .option('-o, --output <path>', 'output path to generated C file', '/dev/stdout')
  .action(function (this: Command, url) {
    const opts = this.opts();
    const outputPath = opts.output as string;
    console.log('TODO', { url, outputPath });
  });

program.parse();
