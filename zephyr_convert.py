import sys
import yaml
import os
import os.path
import re

def main():
    boards_path = sys.argv[1]

    gpio_expression = re.compile('gpios = <&gpio([0-9]) ([0-9]+)')
    name_expression = re.compile('[^a-zA-Z0-9_]')

    with open('.github/workflows/release-build.yml.template', 'r') as fd:
        release_workflow = yaml.safe_load(fd)

    def parse_board_yaml(path):
        with open(path, 'r') as fd:
            data = yaml.safe_load(fd)

        def find_gpios(dts_data, compatible, mode):
            lines = dts_data.splitlines()
            start_line = None
            for i in range(len(lines)):
                line = lines[i].strip()
                if line.startswith(f'compatible = "{compatible}"'):
                    start_line = i + 1
                    break
            if start_line is None:
                return False

            for i in range(start_line, len(lines)):
                line = lines[i].strip()
                # already in the next section
                if line.startswith('compatible = '):
                    break

                if not line.startswith('gpios = '):
                    continue

                match = gpio_expression.search(line)

                if match is None:
                    print(f'error parsing line {i}')
                    continue

                pin_bank = int(match.group(1))
                pin_index = int(match.group(2))
                active_low = 'GPIO_ACTIVE_LOW' in line

                pin = (pin_bank * 32) + pin_index


                yield (pin, line, (
                    f'#ifndef GPIO_CONFIGURATION_PIN_{pin}_MODE',
                    f'#define GPIO_CONFIGURATION_PIN_{pin}_MODE {mode}',
                    f'#define GPIO_CONFIGURATION_PIN_{pin}_INVERT {int(active_low)}',
                ))

        def find_leds(dts_data):
            return map(
                lambda set: set[2] + ('#endif', ), 
                find_gpios(dts_data, 'gpio-leds', 2)
            )

        def find_buttons(dts_data):
            for dataset in find_gpios(dts_data, 'gpio-keys', 1):
                pull = 0
                if 'GPIO_PULL' in dataset[1]:
                    pull = 1 if 'GPIO_PULL_UP' in dataset[1] else 2

                yield dataset[2] + (f'#define GPIO_CONFIGURATION_PIN_{dataset[0]}_PULL {pull}', '#endif')

        def extract_pins(path, board, soc):
            folder = os.path.dirname(path)

            all_files = os.listdir(folder)

            dts_files = [f for f in all_files if f.endswith('.dts') or f.endswith('.dtsi')]

            for dts_file in dts_files:
                full_path = f'{folder}/{dts_file}'

                with open(full_path, 'r') as fd:
                    print(full_path)
                    dts_data = fd.read()

                    lines = []
                    [lines.extend(l) for l in find_leds(dts_data)]
                    [lines.extend(l) for l in find_buttons(dts_data)]

                    if len(lines) == 0:
                        continue

                    chip_identifier = f'{board.get("vendor", "other")}_{board["name"]}'
                    chip_identifier = re.sub(r"[^a-zA-Z0-9]+", "_", chip_identifier)
                    filename = f'src/config/bsp/generated/{chip_identifier}.h'
                    with open(filename, 'w') as fs:
                        for line in lines:
                            fs.write(f'{line}\n')

                    build_uf2 = 'xiao' in chip_identifier or 'promicro' in chip_identifier

                    release_workflow['jobs'][f'build_{chip_identifier}'] = {
                        'uses': './.github/workflows/build.yml',
                        'with': {
                            'chip': soc.upper(),
                            'publish_hex_no_crystal': False,
                            'publish_hex_crystal': True,
                            'publish_zip_no_crystal': False,
                            'publish_zip_no_crystal_with_sd': False,
                            'publish_zip_crystal': False,
                            'publish_zip_crystal_with_sd': not build_uf2,
                            'publish_uf2': build_uf2,
                            'board': chip_identifier,
                        }
                    }

                    break
            print(f'error at {full_path}')

        def find_nrf(path, board):
            socs = board['socs']
            for soc in socs:
                name = soc['name']
                if name in ['nrf51822', 'nrf52832', 'nrf52840']:
                    extract_pins(path, board, name)
                    return
                continue
        
        if 'boards' in data:
            boards = data['boards']
            for board in boards:
                find_nrf(path, board)
        else:
            find_nrf(path, data['board'])
            return

    def parse_dir(path):
        if os.path.isdir(path):
            for file in os.listdir(path):
                parse_dir(f'{path}/{file}')
            return

        elif path.endswith('/board.yml'):
            parse_board_yaml(path)

    parse_dir(boards_path)

    with open('.github/workflows/release-build.yml', 'w') as fd:
        yaml.dump(release_workflow, fd)
    

if __name__ == "__main__":
    main()