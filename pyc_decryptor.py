import os
import zlib
import rotor
import marshal
import binascii
import argparse
import pymarshal

class PYCEncryptor(object):
    def __init__(self):
        # 使用整合后的操作码映射表
        self.opcode_decrypt_map = {
            1: 1,      # 从映射表2
            43: 63,    # 从映射表2
            45: 82,    # 从映射表2
            57: 83,    # 从映射表2
            94: 120,   # 从映射表2
            96: 107,   # 从映射表2
            109: 116,  # 注意：映射表1中是 109:101，映射表2中是 109:116，这里使用映射表2
            110: 109,  # 从映射表2
            117: 116,  # 两个表都有，一致
            122: 124,  # 从映射表2
            130: 124,  # 注意：映射表1中是 130:124，映射表2中是 130:106，这里使用映射表1
            131: 100,  # 两个表都有，一致
            133: 114,  # 从映射表2
            134: 132,  # 从映射表2
            135: 105,  # 从映射表2
            142: 100,  # 从映射表2
            151: 124,  # 从映射表2
            159: 121,  # 从映射表2
            162: 100,  # 从映射表2
            165: 125,  # 从映射表2
            170: 109,  # 注意：映射表1中是 170:109，映射表2中是 170:90，这里使用映射表1
            171: 124,  # 从映射表2
            174: 101,  # 从映射表2
            175: 106,  # 两个表都有，一致
            176: 100,  # 从映射表2
            178: 124,  # 从映射表2
            179: 100,  # 两个表都有，一致
            187: 108   # 两个表都有，一致
        }
        
        # 生成加密映射表（解密映射表的反向映射）
        self.opcode_encrypt_map = {v: k for k, v in self.opcode_decrypt_map.items()}
        
        self.pyc27_header = "\x03\xf3\x0d\x0a\x00\x00\x00\x00"

    def _decrypt_file(self, filename):
        os.path.splitext(filename)
        content = open(filename).read()
        try:
            m = pymarshal.loads(content)
        except:
            try:
                m = marshal.loads(content)
            except Exception as e:
                print("[!] error: %s" % str(e))
                return None
        return m.co_filename.replace('\\', '/'), pymarshal.dumps(m, self.opcode_decrypt_map)

    def decrypt_file(self, input_file, output_file=None):
        result = self._decrypt_file(input_file)
        if not result:
            return
        pyc_filename, pyc_content = result
        if not output_file:
            output_file = os.path.basename(pyc_filename) + '.pyc'
        with open(output_file, 'wb') as fd:
            fd.write(self.pyc27_header + pyc_content)

def main():
    parser = argparse.ArgumentParser(description='onmyoji py decrypt tool')
    parser.add_argument("INPUT_NAME", help='input file')
    parser.add_argument("OUTPUT_NAME", help='output file')
    args = parser.parse_args()
    
    encryptor = PYCEncryptor()
    encryptor.decrypt_file(args.INPUT_NAME, args.OUTPUT_NAME)

if __name__ == '__main__':
    main()
