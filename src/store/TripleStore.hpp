#ifndef TNT_STORE_TRIPLESTORE
#define TNT_STORE_TRIPLESTORE

#include "store/RDF/TermStore.hpp"
#include "store/RDF/NTripleParser.hpp"
#include "../tensor/hypertrie/BoolHyperTrie.hpp"


namespace tnt::store {
    class TripleStore {
        using key_part_t = tnt::util::types::key_part_t;
        using BoolHyperTrie =tnt::tensor::hypertrie::BoolHyperTrie;
        TermStore termIndex{};
        BoolHyperTrie trie{3};

    public:
        const TermStore &getTermIndex() const {
            return termIndex;
        }

        void loadRDF(std::string file_path) {
            size_t count = 0;
            for (auto &&[subject, predicate, object] : NTripleParser{file_path}) {
                const key_part_t &subject_id = termIndex[std::move(subject)];
                const key_part_t &predicate_id = termIndex[std::move(predicate)];
                const key_part_t &object_id = termIndex[std::move(object)];
                // the parser checks already if subject, predicate and object have a valid note:type
                trie.set({subject_id, predicate_id, object_id}, true);
                ++count;
            }
            std::cout << "Loaded " << count << " triples." << std::endl;
        }

        void add(std::tuple<std::string, std::string, std::string> triple) {
            add(parse(std::get<0>(triple)), parse(std::get<1>(triple)), parse(std::get<2>(triple)));
        }

        void add(std::tuple<std::unique_ptr<Term>, std::unique_ptr<Term>, std::unique_ptr<Term>> triple) {
            add(std::move(std::get<0>(triple)), std::move(std::get<1>(triple)), std::move(std::get<2>(triple)));
        }

        inline void add(std::unique_ptr<Term> subject, std::unique_ptr<Term> predicate, std::unique_ptr<Term> object) {
            if (subject->type() != Term::NodeType::Literal and predicate->type() == Term::NodeType::URI) {
                const key_part_t &subject_id = termIndex[std::move(subject)];
                const key_part_t &predicate_id = termIndex[std::move(predicate)];
                const key_part_t &object_id = termIndex[std::move(object)];
                trie.set({subject_id, predicate_id, object_id}, true);
            } else
                throw std::invalid_argument{
                        "Subject or predicate of the triple have a term type that is not allowed there."};
        }

        bool contains(std::tuple<std::string, std::string, std::string> triple) {
            const std::unique_ptr<Term> &subject = parse(std::get<0>(triple));
            const std::unique_ptr<Term> &predicate = parse(std::get<1>(triple));
            const std::unique_ptr<Term> &object = parse(std::get<2>(triple));
            return termIndex.contains(subject) and termIndex.contains(predicate) and termIndex.contains(object);
        }

        void query(std::string sparql) {

        }

    };
};

#endif //TNT_STORE_TRIPLESTORE